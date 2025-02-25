//
// Created by RGAA on 25/02/2025.
//

#include "relay_peer_mgr.h"
#include "relay_peer.h"

namespace tc
{

    RelayPeerManager::RelayPeerManager(const std::shared_ptr<RelayContext>& ctx) {
        context_ = ctx;
    }

    void RelayPeerManager::AddPeer(const std::shared_ptr<RelayPeer>& peer) {
        peers_.Insert(peer->client_id_, peer);
    }

    std::shared_ptr<RelayPeer> RelayPeerManager::RemovePeer(const std::string& client_id) {
        auto peer = peers_.Remove(client_id);
        return peer.has_value() ? peer.value() : nullptr;
    }

    std::weak_ptr<RelayPeer> RelayPeerManager::FindPeer(const std::string& client_id) {
        std::weak_ptr<RelayPeer> target_peer;
        peers_.ApplyAllCond([&](const std::string& cid, const std::shared_ptr<RelayPeer>& peer) {
            if (cid == client_id) {
                target_peer = peer;
                return true;
            }
            return false;
        });
        return target_peer;
    }
}