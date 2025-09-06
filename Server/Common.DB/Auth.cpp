#include "Auth.h"
#include "SqlUtil.h"
#include <memory>
#include <random>
#include <optional>

#include <mysql/jdbc.h>
#include <cppconn/exception.h>

using namespace CommonDB;

static std::string RandomHex(size_t n) {
    static std::mt19937_64 rng{ std::random_device{}() };
    static const char* hex = "0123456789abcdef";
    std::string out; out.resize(n);
    for (size_t i = 0; i < n; i++) out[i] = hex[rng() & 0xF];
    return out;
}

std::optional<AccountRow>Auth::FindByUsername(const std::string& username) {
    try {
        db_.UseAuth();
        std::unique_ptr<sql::PreparedStatement> ps(
            db_.conn()->prepareStatement(
                "SELECT account_id, username, CAST(password_hash AS CHAR) AS password_hash, "
                "password_algo, status "
                "FROM accounts WHERE username=? LIMIT 1"));
        ps->setString(1, username);
        std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
        if (!rs->next()) return std::nullopt;

        AccountRow r;
        r.account_id = rs->getUInt64("account_id");
        r.username = rs->getString("username");
        r.password_hash = rs->getString("password_hash");
        r.password_algo = rs->getString("password_algo");
        r.status = rs->getString("status");
        return r;
    }
    catch (const sql::SQLException& e) {
        // 로깅: SqlUtil::Explain(e)
        return std::nullopt;
    }
}

bool Auth::CreateAccount(const std::string& username, const std::string& password) {
    try {
        db_.UseAuth();

        // 1) 중복 확인
        {
            std::unique_ptr<sql::PreparedStatement> chk(
                db_.conn()->prepareStatement("SELECT 1 FROM accounts WHERE username=? LIMIT 1"));
            chk->setString(1, username);
            std::unique_ptr<sql::ResultSet> rs(chk->executeQuery());
            if (rs->next()) return false; // 이미 존재
        }

        // 2) INSERT (패스워드 해시는 DB에서 계산해서 VARBINARY(32)로 저장)
        {
            std::unique_ptr<sql::PreparedStatement> ins(
                db_.conn()->prepareStatement(
                    "INSERT INTO accounts(username, password_hash, password_algo, status) "
                    "VALUES(?, UNHEX(SHA2(?,256)), 'sha256', 'ACTIVE')"));
            ins->setString(1, username);
            ins->setString(2, password);
            ins->executeUpdate();
        }
        return true;
    }
    catch (const sql::SQLException&) {
        return false; // 필요하면 로그
    }
}

uint64_t Auth::VerifyPasswordAndGetId(const std::string& username, const std::string& password) {
    try {
        db_.UseAuth();
        // username + password 매칭을 DB에서 처리 (sha256 버전)
        std::unique_ptr<sql::PreparedStatement> ps(
            db_.conn()->prepareStatement(
                "SELECT account_id FROM accounts "
                "WHERE username=? AND password_algo='sha256' AND password_hash=UNHEX(SHA2(?,256)) "
                "AND status='ACTIVE' LIMIT 1"));
        ps->setString(1, username);
        ps->setString(2, password);
        std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
        if (rs->next()) return rs->getUInt64(1);
        return 0;
    }
    catch (const sql::SQLException&) {
        return 0;
    }
}

std::string Auth::CreateSession(uint64_t accountId, const std::string& ip,
    const std::string& ua, int ttlSeconds) {
    db_.UseAuth();
    SqlUtil::Tx tx(db_.conn());
    std::string access = RandomHex(64);
    std::string refresh = RandomHex(64);
    std::unique_ptr<sql::PreparedStatement> ps(
        db_.conn()->prepareStatement(
            "INSERT INTO sessions(account_id, access_token, refresh_token, "
            "issued_at, expires_at, last_ip, user_agent) "
            "VALUES(?,?,?, NOW(), DATE_ADD(NOW(), INTERVAL ? SECOND), ?, ?)"));
    ps->setUInt64(1, accountId);
    ps->setString(2, access);
    ps->setString(3, refresh);
    ps->setInt(4, ttlSeconds);
    ps->setString(5, ip);
    ps->setString(6, ua);
    ps->executeUpdate();
    tx.Commit();
    return access;
}

void Auth::InvalidateSession(const std::string& accessToken) {
    db_.UseAuth();
    std::unique_ptr<sql::PreparedStatement> ps(
        db_.conn()->prepareStatement("DELETE FROM sessions WHERE access_token=?"));
    ps->setString(1, accessToken);
    ps->executeUpdate();
}
