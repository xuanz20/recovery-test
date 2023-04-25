#ifndef DB_H
#define DB_H
#include <mysqlx/xdevapi.h>
#include <hiredis.h>
#include <string>

class DB {
    private:
        mysqlx::Session sess;
        redisContext *ctx;
    public:
        DB(std::string, std::string);
        void get(std::string, std::string*);
        void set(std::string, std::string);
        void crash();
        void recovery();
        ~DB();
};

#endif