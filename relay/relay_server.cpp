//
// Created by RGAA on 24/02/2025.
//

#include "relay_server.h"
#include "tc_common_new/log.h"
#include "relay_context.h"
#include "relay_session.h"
#include "relay_messages.h"
#include "relay_session.h"
#include "handler/device_handler.h"
#include "handler/room_handler.h"

namespace tc
{

    RelayServer::RelayServer(const std::shared_ptr<RelayContext>& ctx) : WsServer() {
        context_ = ctx;
    }

    void RelayServer::Init() {
        msg_listener_ = context_->CreateListener();
        InitMessageListener();
    }

    void RelayServer::Start() {
        // 1. prepare
        WsServer::Prepare({
            {"context", context_},
            {"server", std::dynamic_pointer_cast<RelayServer>(shared_from_this())}
        });

        // 2. add relay path
        WsServer::AddWebsocketRouter<RelaySession>("/relay");

        // 3. init handlers
        InitHandlers();

        // finally. start it
        WsServer::Start("0.0.0.0", 20481);
    }

    void RelayServer::Exit() {
        WsServer::Exit();
        exit_ = true;
    }

    std::shared_ptr<RelayContext> RelayServer::GetContext() {
        return context_;
    }

    void RelayServer::InitMessageListener() {
        msg_listener_->Listen<RlMsgTimer1S>([=, this](const RlMsgTimer1S& msg) {

        });

        msg_listener_->Listen<RlMsgTimer5S>([=, this](const RlMsgTimer5S& msg) {

        });
    }

    void RelayServer::InitHandlers() {
        auto sh_this = std::dynamic_pointer_cast<RelayServer>(shared_from_this());
        handlers_.insert({ "client", std::make_shared<DeviceHandler>(sh_this) });
        handlers_.insert({ "room", std::make_shared<RoomHandler>(sh_this) });

        for (const auto& [k, handler] : handlers_) {
            handler->RegisterPaths();
        }
    }

    std::weak_ptr<RelaySession> RelayServer::FindSessionByDeviceId(const std::string& device_id) {
        std::weak_ptr<RelaySession> target_sess;
        sessions_.VisitAllCond([&](const int64_t socket_fd, const std::shared_ptr<WsSession>& sess) -> bool {
            auto rs = std::dynamic_pointer_cast<RelaySession>(sess);
            if (rs == nullptr) {
                return false;
            }
            if (device_id == rs->GetDeviceId()) {
                target_sess = rs;
                return true;
            }
            return false;
        });
        return target_sess;
    }
}