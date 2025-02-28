//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_SESSION_H
#define GAMMARAYSERVER_RELAY_SESSION_H

#include "tc_common_new/ws_server.h"
#include "relay_message.pb.h"

namespace tc
{

    class RelayDevice;
    class RelayDeviceManager;
    class RelayRoom;
    class RelayRoomManager;
    class RelayContext;

    class RelaySession : public WsSession, public std::enable_shared_from_this<RelaySession> {
    public:
        void OnConnected() override;
        void OnDisConnected() override;
        void OnBinMessage(std::string_view data) override;

    private:
        void ProcessRelayMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data);
        void ProcessHelloMessage(std::shared_ptr<RelayMessage>&& msg);
        void ProcessHeartbeatMessage(std::shared_ptr<RelayMessage>&& msg);
        void ProcessRelayTargetMessage(std::shared_ptr<RelayMessage>&& msg);
        void ProcessCreateRoomMessage(std::shared_ptr<RelayMessage>&& msg);
        void ProcessRequestControlMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data);
        void ProcessRequestControlRespMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayRoomManager> room_mgr_ = nullptr;
        std::shared_ptr<RelayDeviceManager> client_mgr_ = nullptr;
        std::string device_id_;
    };

}

#endif //GAMMARAYSERVER_RELAY_SESSION_H
