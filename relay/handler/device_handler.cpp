//
// Created by RGAA on 27/02/2025.
//

#include "device_handler.h"
#include "tc_common_new/log.h"
#include "relay/relay_server.h"
#include "relay/relay_context.h"
#include "relay/relay_device_mgr.h"
#include "relay/relay_device.h"

namespace tc
{

    DeviceHandler::DeviceHandler(const std::shared_ptr<RelayServer>& srv) {
        server_ = srv;
        context_ = srv->GetContext();
        client_mgr_ = context_->GetClientManager();
    }

    void DeviceHandler::RegisterPaths() {
        server_->AddHttpGetRouter("/query/device/count",
            [=, this](const auto& path, http::web_request& req, http::web_response& resp) {
                auto params = GetQueryParams(req.query());
                this->SendOkJson(resp, std::to_string(client_mgr_->GetClientCount()));
        });

        server_->AddHttpGetRouter("/query/devices",
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

                auto clients = client_mgr_->FindDevices(page, page_size);
                json obj_array = json::array();
                for (const auto& client : clients) {
                    json obj;
                    obj["device_id"] = client->device_id_;
                    obj_array.push_back(obj);
                }

                this->SendOkJson(resp, obj_array.dump());
        });
    }

    std::string DeviceHandler::GetErrorMessage(int code) {
        if (code == 1) {
            return "";
        }
        else {
            return BaseHandler::GetErrorMessage(code);
        }
    }

}