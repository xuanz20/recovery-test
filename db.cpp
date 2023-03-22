#include "db.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

DB::DB(std::string user, std::string pass): 
sess(mysqlx::Session("localhost", 33060, user, pass)), redis_conn(sw::redis::Redis("tcp://127.0.0.1:6379"))
{
    sess.sql("CREATE DATABASE IF NOT EXISTS test_db").execute();
    sess.sql("USE test_db").execute();
    sess.sql("DROP TABLE IF EXISTS booklist").execute();
    sess.sql(
        "CREATE TABLE IF NOT EXISTS booklist (name VARCHAR(32), ISBN VARCHAR(96), PRIMARY KEY (name))"
    ).execute();
    // sess.sql("SET GLOBAL innodb_log_file_size = 8589934592").execute();
    std::ifstream file("./data.sql");
    std::string line;
    while(getline(file, line)) {
        sess.sql(line).execute();
    }
    redis_conn.flushdb();
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

void DB::recovery()
{
    while (true) {
        redisContext* context = redisConnect("localhost", 6379);
        if (context != NULL && context->err == 0) {
            redisFree(context);
            break;
        }
        std::cout << "Waiting for Redis server to be set up..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
        std::cout << "Connection restarts!" << std::endl;
    redis_conn = sw::redis::Redis("tcp://127.0.0.1:6379");
    redis_conn.command("CONFIG", "SET", "save", "");
}

DB::~DB()
{
    sess.close();
}