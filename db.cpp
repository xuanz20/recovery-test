#include "db.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

extern std::string conf_path;
extern int key_len;
extern int value_len;

DB::DB(std::string user, std::string pass)
{
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, "localhost", user.c_str(), pass.c_str(), NULL, 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    ctx = (redisContext *)redisConnect("localhost", 6379);
    mysql_query(conn, "CREATE DATABASE IF NOT EXISTS test_db");
    mysql_select_db(conn, "test_db");
    mysql_query(conn, "DROP TABLE IF EXISTS test_table");
    char create_table[256];
    sprintf(create_table, "CREATE TABLE IF NOT EXISTS test_table (my_key CHAR(%d), my_value CHAR(%d), PRIMARY KEY (my_key))", key_len, value_len);
    mysql_query(conn, create_table);
    std::ifstream file("./data.sql");
    std::string line;
    while(getline(file, line)) {
        mysql_query(conn, line.c_str());
    }
    redisReply *reply;
    reply = (redisReply *)redisCommand(ctx, "FLUSHDB");
    freeReplyObject(reply);
    reply = (redisReply *)redisCommand(ctx, "CONFIG SET save \"\"");
    freeReplyObject(reply);
}

void DB::get(std::string key, std::string *value) {
    redisReply *reply = (redisReply *)redisCommand(ctx, "GET %s", key.c_str());
    if (reply->type == REDIS_REPLY_NIL) {
        std::string query = "SELECT my_value FROM test_table WHERE my_key='" + key + "'";
        mysql_query(conn, query.c_str());
        MYSQL_RES *result = mysql_store_result(conn);
        if (result == NULL)
            *value = "";
        else {
            MYSQL_ROW row = mysql_fetch_row(result);
            *value = row[0];
            redisCommand(ctx, "SET %s %s", key.c_str(), value->c_str());
        }
        freeReplyObject(reply);
        return;
    } else if (reply->type == REDIS_REPLY_STRING) {
        *value = std::move(reply->str);
        freeReplyObject(reply);
        return;
    } else {
        printf("Command error: %s\n", reply ? reply->str : "NULL");
        freeReplyObject(reply);
        return;
    }
}

void DB::set(std::string key, std::string value) {
    std::string insert = "INSERT INTO test_table VALUES ('" + key + "', '" + value + "') ON DUPLICATE KEY UPDATE my_value='" + value +"'";
    mysql_query(conn, insert.c_str());
    redisCommand(ctx, "SET %s %s", key.c_str(), value.c_str());
}

void DB::crash() {
    redisCommand(ctx, "SHUTDOWN");
}

void DB::recovery() {
    start_server(conf_path);
    ctx = (redisContext *)redisConnect("localhost", 6379);
    redisCommand(ctx, "CONFIG SET save \"\"");
}

DB::~DB() {
    mysql_close(conn);
    redisFree(ctx);
}