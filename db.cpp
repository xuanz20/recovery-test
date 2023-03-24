#include "db.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

extern std::string conf_path;

DB::DB(std::string user, std::string pass, bool rdb = false): 
sess(mysqlx::Session("localhost", 33060, user, pass)), redis_conn(sw::redis::Redis("tcp://127.0.0.1:6379"))
{
    sess.sql("CREATE DATABASE IF NOT EXISTS test_db").execute();
    sess.sql("USE test_db").execute();
    sess.sql("DROP TABLE IF EXISTS booklist").execute();
    sess.sql(
        "CREATE TABLE IF NOT EXISTS booklist (name CHAR(64), ISBN CHAR(192), PRIMARY KEY (name))"
    ).execute();
    std::ifstream file("./data.sql");
    std::string line;
    while(getline(file, line)) {
        sess.sql(line).execute();
    }
    redis_conn.flushdb();
    if (!rdb)
        redis_conn.command("CONFIG", "SET", "save", "");
}

std::string DB::get(std::string name)
{
    std::string result;
    auto reply = redis_conn.get(name);
    if (reply.has_value()) {
        result = reply.value();
    } else {
        auto tmp = sess.sql("SELECT ISBN FROM booklist WHERE name='" + name + "'").execute().fetchOne();
        if (tmp.isNull())
            result = "";
        else {
            result = std::string(tmp[0]);
            redis_conn.set(name, result);
        }
    }
    return result;
}

void DB::set(std::string name, std::string ISBN)
{
    sess.sql(
        "INSERT INTO booklist VALUES ('" + name + "', '" + ISBN + "') ON DUPLICATE KEY UPDATE ISBN='" + ISBN +"'"
    ).execute();
    redis_conn.set(name, ISBN);
}

void DB::crash()
{
    redis_conn.command<std::string>("SHUTDOWN");
}

void DB::recovery(bool rdb = false)
{
    start_server(conf_path);
    redis_conn = sw::redis::Redis("tcp://127.0.0.1:6379");
    if (!rdb)
        redis_conn.command("CONFIG", "SET", "save", "");
}

DB::~DB()
{
    sess.close();
}