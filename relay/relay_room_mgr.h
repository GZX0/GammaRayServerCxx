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
    class RelayDevice;
    class RelayRoom;
    class RelayContext;
    class RelayServer;

    class RelayRoomManager {
    public:
        explicit RelayRoomManager(const std::shared_ptr<RelayContext>& ctx, const std::shared_ptr<RelayServer>& server);
        // success: RelayRoom ptr
        // failed: nullptr
        std::optional<std::shared_ptr<RelayRoom>> CreateRoom(const std::string& device_id, const std::string& remote_device_id);

        //
        std::optional<std::shared_ptr<RelayRoom>> RemoveRoom(const std::string& room_id);

        // found: Room weak ptr
        // not found: std::nullopt
        std::optional<std::weak_ptr<RelayRoom>> FindRoom(const std::string& room_id);

        // found: Peer weak ptr
        // not found: std::nullopt
        std::optional<std::weak_ptr<RelayDevice>> RemoveClientInRoom(const std::string& room_id, const std::string& device_id);

        uint32_t GetRoomCount();

        std::vector<std::weak_ptr<RelayRoom>> FindRooms(int page, int page_size);

        void DestroyCreatedRoomsBy(const std::string& device_id);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayServer> server_ = nullptr;
        ConcurrentHashMap<std::string, std::shared_ptr<RelayRoom>> rooms_;
    };

}

#endif //GAMMARAYSERVER_RELAY_ROOM_MGR_H
