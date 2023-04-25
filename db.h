#ifndef DB_H
#define DB_H
#include <mysql.h>
#include <hiredis.h>
#include <string>

class DB {
    private:
        // mysqlx::Session sess;
        MYSQL *conn;
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