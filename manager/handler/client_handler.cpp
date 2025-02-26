//
// Created by RGAA on 26/02/2025.
//

#include "client_handler.h"
#include "manager/mgr_server.h"
#include "manager/mgr_context.h"
#include "manager/database/mgr_database.h"

namespace tc
{

    ClientHandler::ClientHandler(const std::shared_ptr<MgrServer>& srv) {
        server_ = srv;
        context_ = server_->GetContext();
        database_ = context_->GetDatabase();
    }

    void ClientHandler::RegisterPaths() {
        server_->AddHttpGetRouter("/request/new/client/id",
            [=, this](const auto& path, http::web_request& req, http::web_response& resp) {
            auto params = GetQueryParams(req.query());
            auto opt_hw_info = GetParam(params, "hw_info");
            if (!opt_hw_info.has_value()) {
                return;
            }

            auto hw_info = opt_hw_info.value();
            auto client_id = database_->GenerateNewClientId(hw_info);
            this->SendOkJson(resp, client_id);
        });
    }

}