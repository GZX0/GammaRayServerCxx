//
// Created by RGAA on 24/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_SERVER_H
#define GAMMARAYSERVER_RELAY_SERVER_H

#include "tc_common_new/concurrent_hashmap.h"
#include "tc_common_new/ws_server.h"

namespace tc
{

    class RelayContext;
    class MessageListener;

    class RelayServer : public WsServer {
    public:
        explicit RelayServer(const std::shared_ptr<RelayContext>& ctx);

        void Init();
        void Start();
        void Exit() override;

    private:
        void InitMessageListener();

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        bool exit_ = false;
        std::shared_ptr<MessageListener> msg_listener_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_RELAY_SERVER_H
