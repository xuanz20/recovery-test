#ifndef DB_H
#define DB_H
#include <mysqlx/xdevapi.h>
#include <sw/redis++/redis++.h>
#include <string>

class DB {
    private:
        mysqlx::Session sess;
        sw::redis::Redis redis_conn;
    public:
        DB(std::string, std::string, bool);
        std::string get(std::string);
        void set(std::string, std::string);
        void crash();
        void recovery(bool);
        ~DB();
};

#endif