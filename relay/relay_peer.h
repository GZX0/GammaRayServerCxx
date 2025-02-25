//
// Created by RGAA on 24/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_PEER_H
#define GAMMARAYSERVER_RELAY_PEER_H

#include <string>
#include <memory>

namespace tc
{

    class RelaySession;

    class RelayPeer {
    public:
        void Notify(const std::string& msg) const;

    public:
        std::string client_id_;
        std::string device_id_;
        int64_t last_update_timestamp_ = 0;
        int64_t socket_fd_{0};
        std::shared_ptr<RelaySession> sess_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_RELAY_PEER_H
