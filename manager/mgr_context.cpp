//
// Created by RGAA on 26/02/2025.
//

#include "mgr_context.h"
#include "mgr_messages.h"
#include "mgr_context.h"
#include "database/mgr_database.h"
#include "tc_common_new/message_notifier.h"
#include "tc_common_new/message_looper.h"

namespace tc
{

    MgrContext::MgrContext(const std::shared_ptr<MessageLooper>& looper) {
        this->msg_looper_ = looper;
    }

    bool MgrContext::Init() {
        auto sh_this = shared_from_this();
        msg_notifier_ = std::make_shared<MessageNotifier>();
        mgr_database_ = std::make_shared<MgrDatabase>(sh_this);
        mgr_database_->Init();
        StartTimers();
        return true;
    }

    std::shared_ptr<MessageListener> MgrContext::CreateListener() {
        return msg_notifier_->CreateListener();
    }

    std::shared_ptr<MgrDatabase> MgrContext::GetDatabase() {
        return mgr_database_;
    }

    void MgrContext::PostInMainLoop(std::function<void()>&& task) {
        msg_looper_->Post(std::move(task));
    }

    void MgrContext::StartTimers() {
        timer_ = std::make_shared<asio2::timer>();

        timer_->start_timer("id_1s", 1000, [=, this]() {
            msg_notifier_->SendAppMessage(RlMsgTimer1S {});
        });

        timer_->start_timer("id_5s", 5000, [=, this]() {
            msg_notifier_->SendAppMessage(RlMsgTimer5S {});
        });
    }
}