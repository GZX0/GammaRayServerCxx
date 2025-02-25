//
// Created by RGAA on 24/02/2025.
//

#include "relay_context.h"
#include "relay_room_mgr.h"
#include "relay_peer_mgr.h"
#include "relay_messages.h"
#include "tc_common_new/message_notifier.h"
#include "tc_common_new/message_looper.h"

namespace tc
{

    RelayContext::RelayContext(const std::shared_ptr<MessageLooper>& looper) {
        this->msg_looper_ = looper;
    }

    bool RelayContext::Init() {
        auto sh_this = shared_from_this();
        msg_notifier_ = std::make_shared<MessageNotifier>();
        room_mgr_ = std::make_shared<RelayRoomManager>(sh_this);
        peer_mgr_ = std::make_shared<RelayPeerManager>(sh_this);
        StartTimers();
        return true;
    }

    std::shared_ptr<RelayPeerManager> RelayContext::GetPeerManager() {
        return peer_mgr_;
    }

    std::shared_ptr<RelayRoomManager> RelayContext::GetRoomManager() {
        return room_mgr_;
    }

    std::shared_ptr<MessageListener> RelayContext::CreateListener() {
        return msg_notifier_->CreateListener();
    }

    void RelayContext::PostInMainLoop(std::function<void()>&& task) {
        msg_looper_->Post(std::move(task));
    }

    void RelayContext::StartTimers() {
        timer_ = std::make_shared<asio2::timer>();

        timer_->start_timer("id_1s", 1000, [=, this]() {
            msg_notifier_->SendAppMessage(RlMsgTimer1S {});
        });

        timer_->start_timer("id_5s", 5000, [=, this]() {
            msg_notifier_->SendAppMessage(RlMsgTimer5S {});
        });
    }
}