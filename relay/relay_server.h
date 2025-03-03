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
    class BaseHandler;
    class RelaySession;

    class RelayServer : public WsServer {
    public:
        explicit RelayServer(const std::shared_ptr<RelayContext>& ctx);

        void Init();
        void Start();
        void Exit() override;
        std::shared_ptr<RelayContext> GetContext();

        std::weak_ptr<RelaySession> FindSessionByDeviceId(const std::string& device_id);

    private:
        void InitMessageListener();
        void InitHandlers();

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        bool exit_ = false;
        std::shared_ptr<MessageListener> msg_listener_ = nullptr;
        std::unordered_map<std::string, std::shared_ptr<BaseHandler>> handlers_;
    };

}

#endif //GAMMARAYSERVER_RELAY_SERVER_H
