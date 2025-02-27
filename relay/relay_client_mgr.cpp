//
// Created by RGAA on 25/02/2025.
//

#include "relay_client_mgr.h"
#include "relay_client.h"
#include "tc_common_new/log.h"
#include <format>

namespace tc
{

    RelayClientManager::RelayClientManager(const std::shared_ptr<RelayContext>& ctx) {
        context_ = ctx;
    }

    void RelayClientManager::AddClient(const std::shared_ptr<RelayClient>& client) {
        clients_.Insert(client->client_id_, client);
    }

    std::optional<std::shared_ptr<RelayClient>> RelayClientManager::RemoveClient(const std::string& client_id) {
        return clients_.Remove(client_id);
    }

    std::weak_ptr<RelayClient> RelayClientManager::FindClient(const std::string& client_id) {
        std::weak_ptr<RelayClient> target_client;
        clients_.ApplyAllCond([&](const std::string& cid, const std::shared_ptr<RelayClient>& client) {
            if (cid == client_id) {
                target_client = client;
                return true;
            }
            return false;
        });
        return target_client;
    }

    uint32_t RelayClientManager::GetClientCount() {
        return clients_.Size();
    }

    std::vector<std::weak_ptr<RelayClient>> RelayClientManager::FindClients(int page, int page_size) {
        int begin = std::max(0, page - 1) * page_size;
        int end = begin + page_size;
        auto opt_clients = clients_.QueryRange(begin, end);
        std::vector<std::weak_ptr<RelayClient>> target_clients;
        if (opt_clients.has_value()) {
            auto clients = opt_clients.value();
            for (const auto& c : clients) {
                target_clients.push_back(c);
            }
        }
        return target_clients;
    }
}