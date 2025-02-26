//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_CONTEXT_H
#define GAMMARAYSERVER_MGR_CONTEXT_H

#include <memory>
#include <asio2/asio2.hpp>
#include "tc_common_new/message_notifier.h"

namespace tc
{

    class MessageLooper;
    class MgrDatabase;

    class MgrContext : public std::enable_shared_from_this<MgrContext> {
    public:
        explicit MgrContext(const std::shared_ptr<MessageLooper>& looper);
        bool Init();
        std::shared_ptr<MessageListener> CreateListener();
        std::shared_ptr<MgrDatabase> GetDatabase();

        void PostInMainLoop(std::function<void()>&& task);

        template<typename T>
        void SendAppMessage(const T& m) {
            msg_notifier_->SendAppMessage(m);
        }

    private:
        void StartTimers();

    private:
        std::shared_ptr<MessageLooper> msg_looper_ = nullptr;
        std::shared_ptr<asio2::timer> timer_ = nullptr;
        std::shared_ptr<MessageNotifier> msg_notifier_ = nullptr;
        std::shared_ptr<MgrDatabase> mgr_database_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_MGR_CONTEXT_H
