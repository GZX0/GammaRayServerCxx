//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_ROOM_MGR_H
#define GAMMARAYSERVER_RELAY_ROOM_MGR_H

#include <memory>
#include <optional>
#include "tc_common_new/concurrent_hashmap.h"

namespace tc
{
    class RelayPeer;
    class RelayRoom;
    class RelayContext;

    class RelayRoomManager {
    public:
        explicit RelayRoomManager(const std::shared_ptr<RelayContext>& ctx);
        // success: RelayRoom ptr
        // failed: nullptr
        std::shared_ptr<RelayRoom> CreateRoom(const std::string& client_id, const std::string& remote_client_id);

        // found: Room weak ptr
        // not found: std::nullopt
        std::optional<std::weak_ptr<RelayRoom>> FindRoom(const std::string& room_id);

        // found: Peer weak ptr
        // not found: std::nullopt
        std::optional<std::weak_ptr<RelayPeer>> RemovePeerInRoom(const std::string& room_id, const std::string& peer_id);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        ConcurrentHashMap<std::string, std::shared_ptr<RelayRoom>> rooms_;
    };

}

#endif //GAMMARAYSERVER_RELAY_ROOM_MGR_H
