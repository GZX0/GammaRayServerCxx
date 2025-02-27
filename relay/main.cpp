//
// Created by RGAA on 24/02/2025.
//

#include "relay_context.h"
#include "tc_common_new/log.h"
#include "relay_server.h"
#include "relay_room.h"
#include "relay_client.h"
#include "relay_room_mgr.h"
#include "relay_client_mgr.h"
#include "tc_common_new/message_looper.h"
#include "settings/relay_settings.h"

using namespace tc;

int main(int argc, char** argv) {

    auto log_file_path = std::format("GammaRayRelayServer.log");
    Logger::InitLog(log_file_path, false);

    auto settings = RelaySettings::Instance();
    settings->LoadSettings();
    settings->DumpSettings();
    auto msg_looper = std::make_shared<MessageLooper>(-1);

    auto context = std::make_shared<RelayContext>(msg_looper);
    if (!context->Init()) {
        return -1;
    }

    auto server = std::make_shared<RelayServer>(context);
    server->Init();
    server->Start();

    msg_looper->Loop();

    return 0;
}