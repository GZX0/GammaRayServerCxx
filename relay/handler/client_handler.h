//
// Created by RGAA on 27/02/2025.
//

#ifndef GAMMARAYSERVER_CLIENT_HANDLER_H
#define GAMMARAYSERVER_CLIENT_HANDLER_H

#include "tc_common_new/base_handler.h"

namespace tc
{

    class RelayContext;
    class RelayServer;
    class RelayClientManager;

    class ClientHandler : public BaseHandler {
    public:
        explicit ClientHandler(const std::shared_ptr<RelayServer>& srv);
        void RegisterPaths() override;
        std::string GetErrorMessage(int code) override;

    private:
        std::shared_ptr<RelayServer> server_ = nullptr;
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayClientManager> client_mgr_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_CLIENT_HANDLER_H
