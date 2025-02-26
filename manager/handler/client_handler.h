//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_CLIENT_HANDLER_H
#define GAMMARAYSERVER_CLIENT_HANDLER_H

#include <memory>
#include "tc_common_new/base_handler.h"

namespace tc
{

    class MgrServer;
    class MgrContext;
    class MgrDatabase;

    class ClientHandler : public BaseHandler {
    public:
        explicit ClientHandler(const std::shared_ptr<MgrServer>& srv);
        void RegisterPaths() override;

    private:
        std::shared_ptr<MgrServer> server_ = nullptr;
        std::shared_ptr<MgrContext> context_ = nullptr;
        std::shared_ptr<MgrDatabase> database_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_CLIENT_HANDLER_H
