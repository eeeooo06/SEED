#pragma once
#include "Db.h"
#include <optional>
#include <string>

namespace CommonDB {

    struct AccountRow {
        uint64_t    account_id{};
        std::string username;
        std::string password_hash;  // DB에서 VARBINARY면 CAST해서 가져옴
        std::string password_algo;  // 'bcrypt' | 'argon2id' | ...
        std::string status;         // 'ACTIVE' ...
    };

    class Auth {
    public:
        explicit Auth(Db& db) : db_(db) {}

        std::optional<AccountRow> FindByUsername(const std::string& username);

        // 신규 추가 ↓
       // username 중복 체크 + INSERT (password는 DB에서 UNHEX(SHA2(?,256))로 해시)
       // 성공 시 true, 중복이면 false
        bool CreateAccount(const std::string& username, const std::string& password);

        // (로그인용) username/password가 일치하면 account_id 반환, 아니면 0
        uint64_t VerifyPasswordAndGetId(const std::string& username, const std::string& password);

        // 필요 시: 세션 생성/무효화
        std::string CreateSession(uint64_t accountId, const std::string& ip, const std::string& ua, int ttlSeconds);
        void InvalidateSession(const std::string& accessToken);

    private:
        Db& db_;
    };

} // namespace CommonDB
