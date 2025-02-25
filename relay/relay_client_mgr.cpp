//
// Created by RGAA on 25/02/2025.
//

#include "relay_client_mgr.h"
#include "relay_client.h"

namespace tc
{

    RelayClientManager::RelayClientManager(const std::shared_ptr<RelayContext>& ctx) {
        context_ = ctx;
    }

    void RelayClientManager::AddClient(const std::shared_ptr<RelayClient>& peer) {
        peers_.Insert(peer->client_id_, peer);
    }

    std::shared_ptr<RelayClient> RelayClientManager::RemoveClient(const std::string& client_id) {
        auto peer = peers_.Remove(client_id);
        return peer.has_value() ? peer.value() : nullptr;
    }

    std::weak_ptr<RelayClient> RelayClientManager::FindClient(const std::string& client_id) {
        std::weak_ptr<RelayClient> target_peer;
        peers_.ApplyAllCond([&](const std::string& cid, const std::shared_ptr<RelayClient>& peer) {
            if (cid == client_id) {
                target_peer = peer;
                return true;
            }
            return false;
        });
        return target_peer;
    }
}