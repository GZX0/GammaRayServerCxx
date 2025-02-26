//
// Created by RGAA on 25/02/2025.
//

#include <format>
#include "relay_room_mgr.h"
#include "relay_room.h"
#include "relay_client_mgr.h"
#include "relay_context.h"
#include "relay_client.h"
#include "tc_common_new/log.h"

namespace tc
{

    RelayRoomManager::RelayRoomManager(const std::shared_ptr<RelayContext>& ctx) {
        context_ = ctx;
    }

    std::optional<std::shared_ptr<RelayRoom>> RelayRoomManager::CreateRoom(const std::string& client_id, const std::string& remote_client_id) {
        auto pm = context_->GetPeerManager();
        // check myself
        auto wk_client = pm->FindClient(client_id);
        auto client = wk_client.lock();
        if (!client|| !client->IsAlive()) {
            LOGW("This peer[My Client] is not alive: {}", client_id);
            return std::nullopt;
        }

        // check remote client
        auto wk_remote_client = pm->FindClient(remote_client_id);
        auto remote_client = wk_remote_client.lock();
        if (!remote_client || !remote_client->IsAlive()) {
            LOGW("This peer[Remote Client] is not alive: {}", remote_client_id);
            return std::nullopt;
        }

        auto room = std::make_shared<RelayRoom>();
        room->room_id_ = std::format("{}-{}", client_id, remote_client_id);
        room->client_id_ = client_id;
        room->remote_client_id_ = remote_client_id;
        room->context_ = context_;
        room->clients_.Insert(client_id, client);
        room->clients_.Insert(remote_client_id, remote_client);
        rooms_.Insert(room->room_id_, room);
        return room;
    }

    std::optional<std::weak_ptr<RelayRoom>> RelayRoomManager::FindRoom(const std::string& room_id) {
        std::weak_ptr<RelayRoom> target_room;
        rooms_.ApplyAllCond([&] (const std::string& id, const std::shared_ptr<RelayRoom>& room) {
            if (room_id == id) {
                target_room = room;
                return true;
            }
            return false;
        });
        if (target_room.lock()) {
            return target_room;
        }
        return std::nullopt;
    }

    std::optional<std::weak_ptr<RelayClient>> RelayRoomManager::RemovePeerInRoom(const std::string& room_id, const std::string& peer_id) {
        if (!rooms_.HasKey(room_id)) {
            return std::nullopt;
        }
        auto room = rooms_.Get(room_id);
        if (room->clients_.HasKey(peer_id)) {
            auto target_peer = room->clients_.Get(peer_id);
            return target_peer;
        }
        return std::nullopt;
    }

}