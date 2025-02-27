//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_SERVER_H
#define GAMMARAYSERVER_MGR_SERVER_H

#include <unordered_map>
#include "tc_common_new/concurrent_hashmap.h"
#include "tc_common_new/ws_server.h"

namespace tc
{

    class MgrContext;
    class MessageListener;
    class BaseHandler;
    class MgrSettings;

    class MgrServer : public WsServer {
    public:
        explicit MgrServer(const std::shared_ptr<MgrContext>& ctx);

        void Init();
        void Start();
        void Exit() override;
        std::shared_ptr<MgrContext> GetContext();

    private:
        void InitMessageListener();
        void InitHandlers();

    private:
        MgrSettings* settings_ = nullptr;
        std::shared_ptr<MgrContext> context_ = nullptr;
        bool exit_ = false;
        std::shared_ptr<MessageListener> msg_listener_ = nullptr;
        std::unordered_map<std::string, std::shared_ptr<BaseHandler>> handlers_;
    };

}

#endif //GAMMARAYSERVER_MGR_SERVER_H
