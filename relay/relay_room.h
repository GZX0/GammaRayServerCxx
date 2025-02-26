//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_ROOM_H
#define GAMMARAYSERVER_RELAY_ROOM_H

#include <string>
#include <memory>
#include "tc_common_new/concurrent_hashmap.h"

namespace tc
{

    class RelayClient;
    class RelayContext;

    class RelayRoom {
    public:
        //      client 1
        // -->  client 2
        //      client 3
        void NotifyAll(const std::string& msg);

        // --> client[client id]
        void NotifyTarget(const std::string& client_id, const std::string& msg);

        //      xxx client 1 [client id]
        // -->  client 2
        //      client 3
        void NotifyExcept(const std::string& client_id, const std::string& msg);

        std::vector<std::weak_ptr<RelayClient>> GetClients();

    public:
        std::string client_id_;
        std::string remote_client_id_;
        std::string room_id_;

        ConcurrentHashMap<std::string, std::weak_ptr<RelayClient>> clients_;
        std::shared_ptr<RelayContext> context_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_RELAY_ROOM_H
