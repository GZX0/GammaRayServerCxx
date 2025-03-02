//
// Created by RGAA on 24/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_CONTEXT_H
#define GAMMARAYSERVER_RELAY_CONTEXT_H

#include <memory>
#include <asio2/asio2.hpp>
#include "tc_common_new/message_notifier.h"
#include <sw/redis++/redis++.h>

using namespace sw::redis;

namespace tc
{

    class MessageLooper;
    class RelayDeviceManager;
    class RelayRoomManager;

    class RelayContext : public std::enable_shared_from_this<RelayContext> {
    public:
        explicit RelayContext(const std::shared_ptr<MessageLooper>& looper);
        bool Init();
        std::shared_ptr<RelayDeviceManager> GetClientManager();
        std::shared_ptr<RelayRoomManager> GetRoomManager();
        std::shared_ptr<MessageListener> CreateListener();
        std::shared_ptr<Redis> GetRedis();
        void PostInMainLoop(std::function<void()>&& task);

        template<typename T>
        void SendAppMessage(const T& m) {
            msg_notifier_->SendAppMessage(m);
        }

    private:
        void StartTimers();

    private:
        std::shared_ptr<MessageLooper> msg_looper_ = nullptr;
        std::shared_ptr<RelayDeviceManager> client_mgr_ = nullptr;
        std::shared_ptr<RelayRoomManager> room_mgr_ = nullptr;
        std::shared_ptr<asio2::timer> timer_ = nullptr;
        std::shared_ptr<MessageNotifier> msg_notifier_ = nullptr;
        std::shared_ptr<Redis> redis_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_RELAY_CONTEXT_H
