//
// Created by RGAA on 24/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_PEER_H
#define GAMMARAYSERVER_RELAY_PEER_H

#include <string>

namespace tc
{

    class RelayPeer {
    public:
        void Notify(const std::string& msg);

    public:
        std::string client_id_;

        int64_t socket_fd_{0};
    };

}

#endif //GAMMARAYSERVER_RELAY_PEER_H
