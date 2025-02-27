//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_CLIENT_MGR_H
#define GAMMARAYSERVER_RELAY_CLIENT_MGR_H

#include <memory>
#include "tc_common_new/concurrent_hashmap.h"

namespace tc
{
    class RelayClient;
    class RelayContext;

    class RelayClientManager {
    public:
        explicit RelayClientManager(const std::shared_ptr<RelayContext>& ctx);
        //
        void AddClient(const std::shared_ptr<RelayClient>& client);
        //
        std::optional<std::shared_ptr<RelayClient>> RemoveClient(const std::string& client_id);
        //
        std::weak_ptr<RelayClient> FindClient(const std::string& client_id);

        // total count
        uint32_t GetClientCount();

        std::vector<std::weak_ptr<RelayClient>> FindClients(int page, int page_size);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        ConcurrentHashMap<std::string, std::shared_ptr<RelayClient>> clients_;
    };
}

#endif //GAMMARAYSERVER_RELAY_CLIENT_MGR_H
