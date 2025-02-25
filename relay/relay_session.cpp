//
// Created by RGAA on 25/02/2025.
//

#include "relay_session.h"
#include "relay_room.h"
#include "relay_peer.h"
#include "relay_room_mgr.h"
#include "relay_peer_mgr.h"
#include "relay_context.h"
#include "message/relay_message.pb.h"

namespace tc
{

    void RelaySession::OnConnected() {
        auto opt_ctx = GetVar<std::shared_ptr<RelayContext>>("context");
        if (opt_ctx.has_value()) {
            context_ = opt_ctx.value();
        }
        room_mgr_ = context_->GetRoomManager();
        peer_mgr_ = context_->GetPeerManager();

        auto client_id = GetQueryParam("client_id");
        if (!client_id.has_value()) {
            LOGE("Don't have a client id, will not work!");
            return;
        }
    }

    void RelaySession::OnDisConnected() {

    }

    void RelaySession::OnBinMessage(std::string_view data) {
        auto rl_msg = std::make_shared<RelayMessage>();
        try {
            rl_msg->ParseFromString(data);
        } catch(std::exception& e) {
            LOGE("Parse message failed: {}", e.what());
            return;
        }
        ProcessRelayMessage(std::move(rl_msg));
    }

    void RelaySession::ProcessRelayMessage(std::shared_ptr<RelayMessage>&& msg) {

    }

}