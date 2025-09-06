#include "Directory.h"
#include <memory>

using namespace CommonDB;

std::vector<ServerRow> Directory::FetchOnlineGameServers() {
    db_.UseDirectory();
    std::unique_ptr<sql::PreparedStatement> ps(
        db_.conn()->prepareStatement(
            "SELECT w.display_name, s.name, s.host, s.port "
            "FROM servers s JOIN worlds w ON w.world_id=s.world_id "
            "WHERE s.server_type='GAME' AND s.status='ONLINE' "
            "ORDER BY w.world_id, s.server_id"));
    std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
    std::vector<ServerRow> out;
    while (rs->next()) {
        out.emplace_back(rs->getString(1), rs->getString(2),
            rs->getString(3), rs->getInt(4));
    }
    return out;
}
