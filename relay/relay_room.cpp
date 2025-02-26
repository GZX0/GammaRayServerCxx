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
        clients_.ApplyAll([&](const std::string& id, const std::weak_ptr<RelayClient>& client) {
            auto target_client = client.lock();
            if (target_client) {
                target_client->Notify(msg);
            }
        });
    }

    // --> client[client id]
    void RelayRoom::NotifyTarget(const std::string& client_id, const std::string& msg) {
        clients_.ApplyAllCond([&](const std::string& id, const std::weak_ptr<RelayClient>& client) {
            if (id == client_id) {
                auto target_client = client.lock();
                if (target_client) {
                    target_client->Notify(msg);
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
        clients_.ApplyAll([&](const std::string& id, const std::weak_ptr<RelayClient>& client) {
            if (id == client_id) {
                return;
            }
            auto target_client = client.lock();
            if (target_client) {
                target_client->Notify(msg);
            }
        });
    }

    std::vector<std::weak_ptr<RelayClient>> RelayRoom::GetClients() {
        std::vector<std::weak_ptr<RelayClient>> clients;
        clients_.ApplyAll([&](const auto& k, const auto& client) {
            clients.push_back(client);
        });
        return clients;
    }

}