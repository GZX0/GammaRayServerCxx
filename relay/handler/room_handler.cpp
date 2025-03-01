//
// Created by RGAA on 27/02/2025.
//

#include "room_handler.h"
#include "tc_common_new/log.h"
#include "relay/relay_server.h"
#include "relay/relay_context.h"
#include "relay/relay_room_mgr.h"
#include "relay/relay_room.h"

namespace tc
{

    RoomHandler::RoomHandler(const std::shared_ptr<RelayServer>& srv) {
        server_ = srv;
        context_ = srv->GetContext();
        room_mgr_ = context_->GetRoomManager();
    }

    void RoomHandler::RegisterPaths() {
        server_->AddHttpGetRouter("/query/room/count",
        [=, this](const auto& path, http::web_request& req, http::web_response& resp) {
            auto params = GetQueryParams(req.query());
            this->SendOkJson(resp, std::to_string(room_mgr_->GetRoomCount()));
        });

        server_->AddHttpGetRouter("/query/rooms",
            [=, this](const auto& path, http::web_request& req, http::web_response& resp) {
                auto params = GetQueryParams(req.query());
                auto opt_page = GetParam(params, "page");
                auto opt_page_size = GetParam(params, "page_size");
                if (!opt_page.has_value() || !opt_page_size.has_value()) {
                    LOGE("Must have [page] & [page_size] params.");
                    SendErrorJson(resp, kHandlerErrParams);
                    return;
                }
                int page = std::atoi(opt_page.value().c_str());
                int page_size = std::atoi(opt_page_size.value().c_str());

                auto rooms = room_mgr_->FindRooms(page, page_size);
                json obj_array = json::array();
                for (const auto& room : rooms) {
                    if (auto r = room.lock(); r) {
                        json obj;
                        obj["room_id"] = r->room_id_;
                        obj["device_id"] = r->device_id_;
                        obj_array.push_back(obj);
                    }
                }

                this->SendOkJson(resp, obj_array.dump());
        });
    }

    std::string RoomHandler::GetErrorMessage(int code) {
        if (code == 1) {
            return "";
        }
        else {
            return BaseHandler::GetErrorMessage(code);
        }
    }

}