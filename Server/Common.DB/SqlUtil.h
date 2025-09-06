#pragma once
#include <mysql/jdbc.h>
#include <string>

namespace CommonDB{
    namespace SqlUtil {

        // 예외 메시지 로깅/문자열로 변환
        std::string Explain(const sql::SQLException& e);

        // 간단 트랜잭션 스코프
        class Tx {
        public:
            explicit Tx(sql::Connection* c) : c_(c), active_(true) { c_->setAutoCommit(false); }
            ~Tx() { if (active_) try { c_->rollback(); c_->setAutoCommit(true); } catch (...) {} }
            void Commit() { c_->commit(); c_->setAutoCommit(true); active_ = false; }
        private:
            sql::Connection* c_;
            bool active_;
        };
    }
} // namespace CommonDB::SqlUtil
