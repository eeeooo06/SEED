#include "SqlUtil.h"
#include <sstream>

using namespace CommonDB;

std::string SqlUtil::Explain(const sql::SQLException& e) {
    std::ostringstream oss;
    oss << "SQLState=" << e.getSQLStateCStr()
        << " code=" << e.getErrorCode()
        << " msg=" << e.what();
    return oss.str();
}
