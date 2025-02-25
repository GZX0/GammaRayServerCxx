//
// Created by RGAA on 25/02/2025.
//

#include "relay_session.h"
#include "relay_room.h"
#include "relay_client.h"
#include "relay_room_mgr.h"
#include "relay_client_mgr.h"
#include "relay_context.h"
#include "message/relay_message.pb.h"
#include "tc_common_new/time_ext.h"

namespace tc
{

    void RelaySession::OnConnected() {
        auto opt_ctx = GetVar<std::shared_ptr<RelayContext>>("context");
        if (opt_ctx.has_value()) {
            context_ = opt_ctx.value();
        }
        room_mgr_ = context_->GetRoomManager();
        client_mgr_ = context_->GetPeerManager();

        auto client_id = GetQueryParam("client_id");
        if (!client_id.has_value()) {
            LOGE("Don't have a client id, will not work!");
            return;
        }

        auto device_id = GetQueryParam("device_id");
        if (!device_id.has_value()) {
            LOGE("Don't have a device id, will not work!");
            return;
        }

        this->client_id_ = client_id.value();
        this->device_id_ = device_id.value();

        auto peer = std::make_shared<RelayClient>();
        peer->client_id_ = this->client_id_;
        peer->device_id_ = this->device_id_;
        peer->socket_fd_ = socket_fd_;
        peer->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
        peer->sess_ = shared_from_this();
        client_mgr_->AddClient(peer);
    }

    void RelaySession::OnDisConnected() {
        client_mgr_->RemoveClient(this->device_id_);
    }

    void RelaySession::OnBinMessage(std::string_view data) {
        auto rl_msg = std::make_shared<RelayMessage>();
        try {
            rl_msg->ParseFromString(data);
        } catch(std::exception& e) {
            LOGE("Parse message failed: {}", e.what());
            return;
        }
        ProcessRelayMessage(std::move(rl_msg), data);
    }

    void RelaySession::ProcessRelayMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data) {
        auto type = msg->type();
        if (type == RelayMessageType::kRelayHello) {
            ProcessHelloMessage(std::move(msg));
        }
        else if (type == RelayMessageType::kRelayHeartBeat) {
            ProcessHeartbeatMessage(std::move(msg));
        }
        else if (type == RelayMessageType::kRelayTargetMessage) {
            ProcessRelayTargetMessage(std::move(msg));
        }
        else if (type == RelayMessageType::kRelayCreateRoom) {
            ProcessCreateRoomMessage(std::move(msg));
        }
        else if (type == RelayMessageType::kRelayRequestControl) {
            ProcessRequestControlMessage(std::move(msg), data);
        }
        else if (type == RelayMessageType::kRelayRequestControlResp) {
            ProcessRequestControlRespMessage(std::move(msg), data);
        }
    }

    void RelaySession::ProcessHelloMessage(std::shared_ptr<RelayMessage>&& msg) {
        auto client_id = msg->client_id();
        auto sub = msg->heartbeat();
        auto wk_peer = client_mgr_->FindClient(client_id);
        auto peer = wk_peer.lock();
        if (!peer) {
            LOGE("Can't find peer for: {}", client_id);
            return;
        }
        peer->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
    }

    void RelaySession::ProcessHeartbeatMessage(std::shared_ptr<RelayMessage>&& msg) {
        auto client_id = msg->client_id();
        auto sub = msg->heartbeat();
        auto wk_peer = client_mgr_->FindClient(client_id);
        auto peer = wk_peer.lock();
        if (!peer) {
            LOGE("Can't find peer for: {}", client_id);
            return;
        }
        peer->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
    }

    void RelaySession::ProcessRelayTargetMessage(std::shared_ptr<RelayMessage>&& msg) {
        auto client_id = msg->client_id();
        auto sub = msg->relay();
        const auto& remote_client_id = sub.remote_client_id();
        auto opt_room = room_mgr_->FindRoom(sub.room_id());
        if (!opt_room.has_value()) {
            LOGW("Can't find room for id: {}, request client id: {}", sub.room_id(), client_id);
            return;
        }

        auto wk_room = opt_room.value();
        auto room = wk_room.lock();
        if (!room) {
            LOGW("Room: {} already destroyed.", sub.room_id());
            return;
        }

        room->NotifyTarget(remote_client_id, sub.payload());
    }

    void RelaySession::ProcessCreateRoomMessage(std::shared_ptr<RelayMessage>&& msg) {
        // requester client id
        auto client_id = msg->client_id();
        auto sub = msg->create_room();
        const auto& remote_client_id = sub.remote_client_id();

        auto opt_room = room_mgr_->CreateRoom(client_id, remote_client_id);
        if (opt_room.has_value()) {
            const auto& room = opt_room.value();
        }
        else {

        }
    }

    void RelaySession::ProcessRequestControlMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data) {

    }

    void RelaySession::ProcessRequestControlRespMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data) {

    }
}