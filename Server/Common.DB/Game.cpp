#include "Game.h"
#include "SqlUtil.h"
#include <memory>
#include <stdexcept>

using namespace CommonDB;

bool Game::CreatePlayer(uint64_t accountId, const std::string& worldCode,
    const std::string& name) {
    auto* c = db_.conn();
    SqlUtil::Tx tx(c);
    // world_id 조회
    db_.UseDirectory();
    int worldId = -1;
    {
        std::unique_ptr<sql::PreparedStatement> ps(
            c->prepareStatement("SELECT world_id FROM worlds WHERE world_code=?"));
        ps->setString(1, worldCode);
        std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
        if (!rs->next()) throw std::runtime_error("invalid world");
        worldId = rs->getInt(1);
    }
    // INSERT
    db_.UseGame();
    std::unique_ptr<sql::PreparedStatement> ins(
        c->prepareStatement(
            "INSERT INTO players(account_id, world_id, name, level) VALUES(?,?,?,1)"));
    ins->setUInt64(1, accountId);
    ins->setInt(2, worldId);
    ins->setString(3, name);
    ins->executeUpdate();

    tx.Commit();
    return true;
}

std::vector<PlayerRow> Game::ListPlayersByAccount(uint64_t accountId) {
    db_.UseGame();
    std::unique_ptr<sql::PreparedStatement> ps(
        db_.conn()->prepareStatement(
            "SELECT player_id, account_id, name, level FROM players WHERE account_id=?"));
    ps->setUInt64(1, accountId);
    std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
    std::vector<PlayerRow> out;
    while (rs->next()) {
        out.push_back(PlayerRow{
            rs->getUInt64(1), rs->getUInt64(2), rs->getString(3),
            static_cast<unsigned>(rs->getUInt(4))
            });
    }
    return out;
}
