#include "db.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

extern std::string conf_path;

DB::DB(std::string user, std::string pass) : 
    sess(mysqlx::Session("localhost", 33060, user, pass))
{
    ctx = (redisContext *)redisConnect("localhost", 6379);
    sess.sql("CREATE DATABASE IF NOT EXISTS test_db").execute();
    sess.sql("USE test_db").execute();
    sess.sql("DROP TABLE IF EXISTS test_table").execute();
    sess.sql(
        "CREATE TABLE IF NOT EXISTS test_table (my_key CHAR(64), my_value CHAR(192), PRIMARY KEY (my_key))"
    ).execute();
    std::ifstream file("./data.sql");
    std::string line;
    while(getline(file, line)) {
        sess.sql(line).execute();
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
        auto tmp = sess.sql("SELECT my_value FROM test_table WHERE my_key='" + key + "'").execute().fetchOne();
        if (tmp.isNull())
            *value = "";
        else {
            *value = std::move(std::string(tmp[0]));
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
    sess.sql(
        "INSERT INTO test_table VALUES ('" + key + "', '" + value + "') ON DUPLICATE KEY UPDATE my_value='" + value +"'"
    ).execute();
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
    sess.close();
    redisFree(ctx);
}