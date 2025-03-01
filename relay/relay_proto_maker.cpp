//
// Created by RGAA on 26/02/2025.
//

#include "relay_proto_maker.h"
#include "relay_device.h"

namespace tc
{

    std::string RelayProtoMaker::MakeErrorMessage(const RelayErrorCode& code,
                                        const RelayMessageType& which_message) {
        return MakeErrorMessage(code, GetErrorMessage(code), which_message);
    }

    std::string RelayProtoMaker::MakeErrorMessage(const RelayErrorCode& code,
                                                  const std::string& msg,
                                                  const RelayMessageType& which_message) {
        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayError);
        auto sub = rl_msg.mutable_relay_error();
        sub->set_code(code);
        sub->set_message(msg);
        sub->set_which_message(which_message);
        return rl_msg.SerializeAsString();
    }

    std::string RelayProtoMaker::MakeCreateRoomResp(const std::string& device_id,
                                          const std::string& remote_device_id,
                                          const std::string& room_id,
                                          const std::vector<std::weak_ptr<RelayDevice>>& devices) {
        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayCreateRoomResp);
        auto resp = rl_msg.mutable_create_room_resp();
        resp->set_device_id(device_id);
        resp->set_remote_device_id(remote_device_id);
        resp->set_room_id(room_id);
        return rl_msg.SerializeAsString();
    }

    std::string RelayProtoMaker::GetErrorMessage(const RelayErrorCode& code) {
        if (code == RelayErrorCode::kRelayCodeOk) {
            return "ok";
        }
        else if (code == RelayErrorCode::kRelayCodeClientNotFound) {
            return "client not found";
        }
        else if (code == RelayErrorCode::kRelayCodeRemoteClientNotFound) {
            return "remote client not found";
        }
        else if (code == RelayErrorCode::kRelayCodeCreateRoomFailed) {
            return "create room failed";
        }
        else if (code == RelayErrorCode::kRelayCodeRejectControl) {
            return "reject control";
        }
        else {
            return "unknown error";
        }
    }

}