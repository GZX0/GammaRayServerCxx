//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_SESSION_H
#define GAMMARAYSERVER_RELAY_SESSION_H

#include "tc_common_new/ws_server.h"

namespace tc
{

    class RelayPeer;
    class RelayPeerManager;
    class RelayRoom;
    class RelayRoomManager;
    class RelayContext;
    class RelayMessage;

    class RelaySession : public WsSession {
    public:
        void OnConnected() override;
        void OnDisConnected() override;
        void OnBinMessage(std::string_view data) override;

    private:
        void ProcessRelayMessage(std::shared_ptr<RelayMessage>&& msg);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayRoomManager> room_mgr_ = nullptr;
        std::shared_ptr<RelayPeerManager> peer_mgr_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_RELAY_SESSION_H
