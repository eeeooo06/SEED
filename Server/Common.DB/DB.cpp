#include "DB.h"
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#  include <windows.h>
#endif

using namespace CommonDB;

static void Probe(const char* n) {
#if _DEBUG && defined(_WIN32)
    HMODULE h = LoadLibraryA(n);
    if (!h) std::cerr << "[DLL] Load FAIL " << n << " err=" << GetLastError() << "\n";
    else { std::cout << "[DLL] Load OK " << n << "\n"; FreeLibrary(h); }
#else
    (void)n;
#endif
}

void Db::Normalize(DbConfig& c) {
    const std::string prefix = "tcp://";
    if (c.host.rfind(prefix, 0) == 0) c.host.erase(0, prefix.size());
    size_t pos = c.host.find(':');
    if (pos != std::string::npos) {
        std::string portStr = c.host.substr(pos + 1);
        c.host.erase(pos);
        if (!portStr.empty()) c.port = std::stoi(portStr);
    }
    if (c.host.find(':') != std::string::npos)
        throw std::runtime_error("Host must not contain ':port' after normalize");
}

Db::Db(const DbConfig& cfg) : cfg_(cfg) {
    Normalize(cfg_);
    std::cout << "[DB] connecting to " << cfg_.host << ":" << cfg_.port
        << " as " << cfg_.user << "\n";

    // 의존 DLL 확인 (디버그용)
    Probe("mysqlcppconn-9-vs14.dll");   // ★ 9.x 이름
    Probe("libmysql.dll");
    Probe("libssl-3-x64.dll");
    Probe("libcrypto-3-x64.dll");

    // 1) 드라이버 취득 (9.x JDBC API)
    driver_ = sql::mysql::get_driver_instance();   // ★ 9.x
    if (!driver_) throw std::runtime_error("get_driver_instance returned null");

    try {
        // 2) URI + SSL 비활성 + 서버 공개키 허용
        std::string uri = "tcp://" + cfg_.host + ":" + std::to_string(cfg_.port)
            + "?ssl-mode=DISABLED&getServerPublicKey=true";

        std::cout << "[DB] uri=" << uri << "\n";
        connection_.reset(driver_->connect(uri, cfg_.user, cfg_.pass));
        if (!connection_) throw std::runtime_error("driver->connect returned null");

        // 3) 기본 스키마
        connection_->setSchema("auth");
        std::cout << "[DB] connected OK (uri, ssl disabled)\n";
    }
    catch (const sql::SQLException& e) {
        std::cerr << "[DB] SQLException: " << e.what()
            << " (sqlstate=" << (e.getSQLStateCStr() ? e.getSQLStateCStr() : "NULL")
            << ", code=" << e.getErrorCode() << ")\n";
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "[DB] std::exception: " << e.what() << "\n";
        throw;
    }
}

Db::~Db() = default;

sql::Connection* Db::conn() { return connection_.get(); }

void Db::UseAuth() { connection_->setSchema("auth"); }
void Db::UseDirectory() { connection_->setSchema("directory"); }
void Db::UseGame() { connection_->setSchema("game"); }
