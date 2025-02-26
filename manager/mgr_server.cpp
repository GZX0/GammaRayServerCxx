//
// Created by RGAA on 26/02/2025.
//

#include "mgr_server.h"
#include "mgr_context.h"
#include "mgr_session.h"
#include "mgr_messages.h"
#include "tc_common_new/log.h"
#include "handler/client_handler.h"

namespace tc
{

    MgrServer::MgrServer(const std::shared_ptr<MgrContext>& ctx) : WsServer() {
        context_ = ctx;
    }

    void MgrServer::Init() {
        msg_listener_ = context_->CreateListener();
        InitMessageListener();
    }

    void MgrServer::Start() {
        // 1. prepare
        WsServer::Prepare({
            {"context", context_},
        });

        // 2. add relay path
        WsServer::AddWebsocketRouter<MgrSession>("/mgr");

        // 3. init handlers
        InitHandlers();

        // finally, start it
        WsServer::Start("0.0.0.0", 20581);
    }

    void MgrServer::Exit() {
        WsServer::Exit();
        exit_ = true;
    }

    std::shared_ptr<MgrContext> MgrServer::GetContext() {
        return context_;
    }

    void MgrServer::InitMessageListener() {
        msg_listener_->Listen<RlMsgTimer1S>([=, this](const RlMsgTimer1S& msg) {

        });

        msg_listener_->Listen<RlMsgTimer5S>([=, this](const RlMsgTimer5S& msg) {

        });
    }

    void MgrServer::InitHandlers() {
        auto sh_this = std::dynamic_pointer_cast<MgrServer>(shared_from_this());
        handlers_.insert({ "client", std::make_shared<ClientHandler>(sh_this) });

        for (const auto& [k, handler] : handlers_) {
            handler->RegisterPaths();
        }
    }

}