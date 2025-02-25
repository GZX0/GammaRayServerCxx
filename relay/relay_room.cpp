//
// Created by RGAA on 25/02/2025.
//

#include "relay_room.h"
#include "relay_client.h"

namespace tc
{
    //      client 1
    // -->  client 2
    //      client 3
    void RelayRoom::NotifyAll(const std::string& msg) {
        peers_.ApplyAll([&](const std::string& id, const std::weak_ptr<RelayClient>& peer) {
            auto target_peer = peer.lock();
            if (target_peer) {
                target_peer->Notify(msg);
            }
        });
    }

    // --> client[client id]
    void RelayRoom::NotifyTarget(const std::string& client_id, const std::string& msg) {
        peers_.ApplyAllCond([&](const std::string& id, const std::weak_ptr<RelayClient>& peer) {
            if (id == client_id) {
                auto target_peer = peer.lock();
                if (target_peer) {
                    target_peer->Notify(msg);
                }
                return true;
            }
            return false;
        });
    }

    //      xxx client 1 [client id]
    // -->  client 2
    //      client 3
    void RelayRoom::NotifyExcept(const std::string& client_id, const std::string& msg) {
        peers_.ApplyAll([&](const std::string& id, const std::weak_ptr<RelayClient>& peer) {
            if (id == client_id) {
                return;
            }
            auto target_peer = peer.lock();
            if (target_peer) {
                target_peer->Notify(msg);
            }
        });
    }

}