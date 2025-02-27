//
// Created by RGAA on 26/02/2025.
//

#include "client_handler.h"
#include "manager/mgr_server.h"
#include "manager/mgr_context.h"
#include "manager/database/mgr_device.h"
#include "manager/database/mgr_database.h"

namespace tc
{

    ClientHandler::ClientHandler(const std::shared_ptr<MgrServer>& srv) {
        server_ = srv;
        context_ = server_->GetContext();
        database_ = context_->GetDatabase();
    }

    void ClientHandler::RegisterPaths() {
        server_->AddHttpGetRouter("/request/new/device",
            [=, this](const auto& path, http::web_request& req, http::web_response& resp) {
            auto params = GetQueryParams(req.query());
            auto opt_hw_info = GetParam(params, "hw_info");
            if (!opt_hw_info.has_value()) {
                LOGE("hw info is empty!");
                return;
            }

            auto opt_platform = GetParam(params, "platform");
            std::string platform;
            if (opt_platform.has_value()) {
                platform = opt_platform.value();
            }

            LOGI("New client in thread: {}", GetCurrentThreadId());
            auto hw_info = opt_hw_info.value();
            auto device = database_->GenerateNewDevice(hw_info, platform);
            json obj;
            obj["device_id"] = device->device_id_;
            obj["random_pwd"] = device->random_pwd_;
            this->SendOkJson(resp, obj.dump());
        });
    }

}