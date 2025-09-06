#pragma once
#include "Db.h"
#include <optional>
#include <string>
#include <vector>

namespace CommonDB {

    struct PlayerRow {
        uint64_t player_id{};
        uint64_t account_id{};
        std::string name;
        unsigned level{};
    };

    class Game {
    public:
        explicit Game(Db& db) : db_(db) {}

        bool CreatePlayer(uint64_t accountId, const std::string& worldCode,
            const std::string& name);
        std::vector<PlayerRow> ListPlayersByAccount(uint64_t accountId);

    private:
        Db& db_;
    };

} // namespace CommonDB
