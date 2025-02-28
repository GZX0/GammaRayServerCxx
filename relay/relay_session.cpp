//
// Created by RGAA on 25/02/2025.
//

#include "relay_session.h"
#include "relay_room.h"
#include "relay_device.h"
#include "relay_room_mgr.h"
#include "relay_device_mgr.h"
#include "relay_context.h"
#include "relay_proto_maker.h"
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
        client_mgr_ = context_->GetClientManager();

        auto device_id = GetQueryParam("device_id");
        if (!device_id.has_value()) {
            LOGE("Don't have a device id, will not work!");
            return;
        }

        this->device_id_ = device_id.value();

        auto peer = std::make_shared<RelayDevice>();
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
            if (!rl_msg->ParseFromString(std::string{data})) {
                LOGE("Parse message failed!");
                return;
            }
        } catch(std::exception& e) {
            LOGE("Parse message failed: {}, msg: {}", e.what(), data);
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
        auto device_id = msg->device_id();
        auto sub = msg->heartbeat();
        auto client = client_mgr_->FindDevice(device_id).lock();
        if (!client) {
            LOGE("Can't find my device_id for: {}", device_id);
            // my state is illegal
            auto resp_msg
                = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }
        client->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
    }

    void RelaySession::ProcessHeartbeatMessage(std::shared_ptr<RelayMessage>&& msg) {
        auto device_id = msg->device_id();
        auto sub = msg->heartbeat();
        auto client = client_mgr_->FindDevice(device_id).lock();
        if (!client) {
            LOGE("Can't find my device_id for: {}", device_id);
            auto resp_msg
                    = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }
        client->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
    }

    void RelaySession::ProcessRelayTargetMessage(std::shared_ptr<RelayMessage>&& msg) {
        auto device_id = msg->device_id();
        auto sub = msg->relay();
        const auto& remote_device_id = sub.remote_device_id();
        auto opt_room = room_mgr_->FindRoom(sub.room_id());
        if (!opt_room.has_value()) {
            LOGW("Can't find room for id: {}, request device id: {}", sub.room_id(), device_id);
            return;
        }

        auto wk_room = opt_room.value();
        auto room = wk_room.lock();
        if (!room) {
            LOGW("Room: {} already destroyed.", sub.room_id());
            return;
        }

        room->NotifyTarget(remote_device_id, sub.payload());
    }

    void RelaySession::ProcessCreateRoomMessage(std::shared_ptr<RelayMessage>&& msg) {
        // requester client id
        auto device_id = msg->device_id();
        auto sub = msg->create_room();
        const auto& remote_device_id = sub.remote_device_id();

        auto opt_room = room_mgr_->CreateRoom(device_id, remote_device_id);
        if (opt_room.has_value()) {
            const auto& room = opt_room.value();
            auto resp_msg
                = RelayProtoMaker::MakeCreateRoomResp(device_id, remote_device_id, room->room_id_, room->GetDevices());
            this->PostBinMessage(resp_msg);
        }
        else {
            auto resp_msg
                = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeCreateRoomFailed, msg->type());
            this->PostBinMessage(resp_msg);
        }
    }

    void RelaySession::ProcessRequestControlMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data) {
        // request id
        auto device_id = msg->device_id();
        auto sub = msg->request_control();
        const auto& remote_device_id = sub.remote_device_id();
        // find remote client
        auto remote_client = client_mgr_->FindDevice(remote_device_id).lock();
        if (!remote_client || !remote_client->IsAlive()) {
            LOGE("Can't find client for remote: {}", remote_device_id);
            auto resp_msg
                = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeRemoteClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }

        std::string cpy_data(data.data(), data.size());
        remote_client->Notify(cpy_data);
    }

    void RelaySession::ProcessRequestControlRespMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data) {
        // client response
        auto sub = msg->request_control_resp();
        const auto& requester_id = sub.device_id();
        auto requester_client = client_mgr_->FindDevice(requester_id).lock();
        if (!requester_client || !requester_client->IsAlive()) {
            LOGE("Can't find client for requester: {}", requester_id);
            auto resp_msg
                    = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeRemoteClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }
        std::string cpy_data(data.data(), data.size());
        requester_client->Notify(cpy_data);
    }
}