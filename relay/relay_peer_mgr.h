//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_PEER_MGR_H
#define GAMMARAYSERVER_RELAY_PEER_MGR_H

#include <memory>
#include "tc_common_new/concurrent_hashmap.h"

namespace tc
{
    class RelayPeer;
    class RelayContext;

    class RelayPeerManager {
    public:
        explicit RelayPeerManager(const std::shared_ptr<RelayContext>& ctx);
        void AddPeer(const std::shared_ptr<RelayPeer>& peer);
        std::shared_ptr<RelayPeer> RemovePeer(const std::string& client_id);
        std::weak_ptr<RelayPeer> FindPeer(const std::string& client_id);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        ConcurrentHashMap<std::string, std::shared_ptr<RelayPeer>> peers_;
    };
}

#endif //GAMMARAYSERVER_RELAY_PEER_MGR_H
