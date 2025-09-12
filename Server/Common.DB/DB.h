#pragma once
#include <memory>
#include <string>
#include <mysql/jdbc.h>
#include <cppconn/exception.h>

namespace CommonDB {

    struct DbConfig {
        std::string host = "127.0.0.1"; // IP만
        int         port = 3306;
        std::string user = "root";
        std::string pass = "";
    };

    class Db {
    public:
        explicit Db(const DbConfig& cfg);
        ~Db();

        sql::Connection* conn();

        void UseAuth();
        void UseDirectory();
        void UseGame();

        static void Normalize(DbConfig& c);   // ★ static 선언

    private:
        DbConfig cfg_;
        sql::Driver* driver_ = nullptr;      // ★ 9.x: sql::Driver*
        std::unique_ptr<sql::Connection> connection_;
    };

} // namespace CommonDB
