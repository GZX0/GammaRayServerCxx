//
// Created by RGAA on 24/02/2025.
//

#include "relay_context.h"
#include "relay_room_mgr.h"
#include "relay_device_mgr.h"
#include "relay_messages.h"
#include "tc_common_new/message_notifier.h"
#include "tc_common_new/message_looper.h"

namespace tc
{

    RelayContext::RelayContext(const std::shared_ptr<MessageLooper>& looper, const std::shared_ptr<Redis>& redis) {
        this->msg_looper_ = looper;
        this->redis_ = redis;
    }

    bool RelayContext::Init(const std::shared_ptr<RelayDeviceManager>& dev_mgr,
                            const std::shared_ptr<RelayRoomManager>& room_mgr) {
        auto sh_this = shared_from_this();
        this->device_mgr_ = dev_mgr;
        this->room_mgr_ = room_mgr;

        msg_notifier_ = std::make_shared<MessageNotifier>();

        StartTimers();
        return true;
    }

    std::shared_ptr<RelayDeviceManager> RelayContext::GetClientManager() {
        return device_mgr_;
    }

    std::shared_ptr<RelayRoomManager> RelayContext::GetRoomManager() {
        return room_mgr_;
    }

    std::shared_ptr<MessageListener> RelayContext::CreateListener() {
        return msg_notifier_->CreateListener();
    }

    std::shared_ptr<Redis> RelayContext::GetRedis() {
        return redis_;
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