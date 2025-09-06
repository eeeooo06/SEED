#pragma once
#include "Db.h"
#include <string>
#include <vector>
#include <tuple>

namespace CommonDB {

    using ServerRow = std::tuple<std::string /*world*/, std::string /*name*/,
        std::string /*host*/, int /*port*/>;

    class Directory {
    public:
        explicit Directory(Db& db) : db_(db) {}
        std::vector<ServerRow> FetchOnlineGameServers();
    private:
        Db& db_;
    };

} // namespace CommonDB
