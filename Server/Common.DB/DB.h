#pragma once
#include <memory>
#include <string>
#include <mysql/jdbc.h>
#include <cppconn/exception.h>

namespace CommonDB {

    struct DbConfig {
        std::string host = "tcp://222.232.36.150:3306"; // or "tcp://222.232.36.150:3306"
        std::string user = "GM_Admin";
        std::string pass = "GameMaster!@Admin";
        // 기본 스키마는 필요할 때 setSchema로 전환
    };

    class Db {
    public:
        explicit Db(const DbConfig& cfg);
        ~Db();

        // 생 커넥션 포인터(소유권은 Db가 가짐)
        sql::Connection* conn();

        // 스키마 전환 헬퍼
        void UseAuth();
        void UseDirectory();
        void UseGame();

    private:
        DbConfig cfg_;
        sql::mysql::MySQL_Driver* driver_;                      // 소유권 X (싱글턴)
        std::unique_ptr<sql::Connection> connection_;           // 소유권 O
    };

} // namespace CommonDB
