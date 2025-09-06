#include "DB.h"
#include <stdexcept>

using namespace CommonDB;

Db::Db(const DbConfig& cfg) : cfg_(cfg), driver_(nullptr) {
    driver_ = sql::mysql::get_mysql_driver_instance();
    sql::ConnectOptionsMap props;
    props["hostName"] = sql::SQLString("222.232.36.150"); // ← IP만! "tcp://..." 금지
    props["port"] = 3306;
    props["userName"] = sql::SQLString(cfg_.user);
    props["password"] = sql::SQLString(cfg_.pass);

    // 네트워크/SSL 진단을 위해 우선 PREFERRED로
    props["sslMode"] = sql::SSL_MODE_PREFERRED;
    props["OPT_CONNECT_TIMEOUT"] = 5;      // 5초 타임아웃

    connection_.reset(driver_->connect(props));
    connection_->setSchema("auth");
}

Db::~Db() = default;

sql::Connection* Db::conn() { return connection_.get(); }

void Db::UseAuth() { connection_->setSchema("auth"); }
void Db::UseDirectory() { connection_->setSchema("directory"); }
void Db::UseGame() { connection_->setSchema("game"); }
