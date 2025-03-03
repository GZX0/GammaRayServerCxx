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
#include "relay_message.pb.h"
#include "relay_server.h"
#include "tc_common_new/time_ext.h"

namespace tc
{

    bool RelaySession::OnConnected() {
        context_ = GetVar<std::shared_ptr<RelayContext>>("context").value_or(nullptr);
        if (!context_) {
            return false;
        }

        server_ = GetVar<std::shared_ptr<RelayServer>>("server").value_or(nullptr);
        if (!server_) {
            return false;
        }

        room_mgr_ = context_->GetRoomManager();
        device_mgr_ = context_->GetClientManager();

        auto device_id = GetQueryParam("device_id");
        if (!device_id.has_value()) {
            LOGE("Don't have a device id, will not work!");
            return false;
        }

        this->device_id_ = device_id.value();

        auto device = std::make_shared<RelayDevice>();
        device->device_id_ = this->device_id_;
        device->socket_fd_ = socket_fd_;
        device->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
        device->sess_ = shared_from_this();
        device_mgr_->AddDevice(device);
        LOGI("--> New session: {}", this->device_id_);

        return true;
    }

    void RelaySession::OnDisConnected() {
        room_mgr_->DestroyCreatedRoomsBy(this->device_id_);
        device_mgr_->RemoveDevice(this->device_id_);
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
            ProcessRelayTargetMessage(std::move(msg), data);
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
        auto from_device_id = msg->from_device_id();
        auto sub = msg->hello();
        auto client = device_mgr_->FindDevice(from_device_id).lock();
        if (!client) {
            LOGE("Can't find my device_id for: {}", from_device_id);
            // my state is illegal
            auto resp_msg
                = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }
        client->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
    }

    void RelaySession::ProcessHeartbeatMessage(std::shared_ptr<RelayMessage>&& msg) {
        auto device_id = msg->from_device_id();
        auto sub = msg->heartbeat();
        auto client = device_mgr_->FindDevice(device_id).lock();
        if (!client) {
            LOGE("Can't find my device_id for: {}", device_id);
            auto resp_msg
                    = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }
        client->last_update_timestamp_ = (int64_t)TimeExt::GetCurrentTimestamp();
    }

    void RelaySession::ProcessRelayTargetMessage(std::shared_ptr<RelayMessage>&& msg, std::string_view data) {
        auto from_device_id = msg->from_device_id();
        auto sub = msg->relay();
        int room_size = sub.room_ids_size();
        for (int i = 0; i < room_size; i++) {
            auto room_id = sub.room_ids().at(i);
            auto opt_room = room_mgr_->FindRoom(room_id);
            if (!opt_room.has_value()) {
                LOGW("Can't find room for id: {}, request device id: {}", room_id, from_device_id);
                continue;
            }
            auto room = opt_room.value().lock();
            if (!room) {
                LOGW("Can't find room for id: {}, request device id: {}", room_id, from_device_id);
                continue;
            }
            //LOGI("Relay in room: {}", room_id);
            room->NotifyExcept(from_device_id, std::string{data});
        }
    }

    void RelaySession::ProcessCreateRoomMessage(std::shared_ptr<RelayMessage>&& msg) {
        // requester client id
        auto sub = msg->create_room();
        const auto& device_id = sub.device_id();
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
        auto sub = msg->request_control();
        const auto& device_id = sub.device_id();
        const auto& remote_device_id = sub.remote_device_id();
        // find remote client
        auto remote_client = device_mgr_->FindDevice(remote_device_id).lock();
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
        // requester
        const auto& req_device_id = sub.device_id();
        auto req_device = device_mgr_->FindDevice(req_device_id).lock();
        if (!req_device || !req_device->IsAlive()) {
            LOGE("Can't find client for requester: {}", req_device_id);
            auto resp_msg
                    = RelayProtoMaker::MakeErrorMessage(RelayErrorCode::kRelayCodeRemoteClientNotFound, msg->type());
            this->PostBinMessage(resp_msg);
            return;
        }
        std::string cpy_data(data.data(), data.size());
        req_device->Notify(cpy_data);

        // under control
        if (sub.under_control()) {
            // 0. make the two in same room
            auto room_id = sub.room_id();
            auto opt_room = room_mgr_->FindRoom(room_id);
            if (!opt_room.has_value()) {
                LOGE("Room disappeared: {}", room_id);
                return;
            }

            auto room = opt_room.value().lock();
            if (!room) {
                LOGE("Room invalid: {}", room_id);
                return;
            }

            //
            auto resp_device = device_mgr_->FindDevice(sub.remote_device_id()).lock();
            if (!resp_device) {
                LOGE("Resp device invalid: {}", sub.remote_device_id());
                return;
            }

            room->AddDevice(req_device);
            room->AddDevice(resp_device);

            RelayMessage rl_msg;
            rl_msg.set_type(RelayMessageType::kRelayRoomPrepared);
            auto rp = rl_msg.mutable_room_prepared();
            rp->set_room_id(room_id);
            rp->set_device_id(sub.device_id());
            rp->set_remote_device_id(sub.remote_device_id());
            auto rp_msg = rl_msg.SerializeAsString();

            // 1. notify requester
            req_device->Notify(rp_msg);

            // 2. notify remote
            resp_device->Notify(rp_msg);
        }
    }
}