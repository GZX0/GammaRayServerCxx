//
// Created by RGAA on 24/02/2025.
//

#include "relay_context.h"
#include "tc_common_new/log.h"
#include "relay_server.h"
#include "relay_room.h"
#include "relay_device.h"
#include "relay_room_mgr.h"
#include "relay_device_mgr.h"
#include "tc_common_new/message_looper.h"
#include "settings/relay_settings.h"
#include <sw/redis++/redis++.h>

using namespace tc;
using namespace sw::redis;

int main(int argc, char** argv) {

    auto log_file_path = std::format("GammaRayRelayServer.log");
    Logger::InitLog(log_file_path, false);

    auto settings = RelaySettings::Instance();
    settings->LoadSettings();
    settings->DumpSettings();
    auto msg_looper = std::make_shared<MessageLooper>(-1);

    // construct Redis
    auto redis = std::make_shared<Redis>("tcp://127.0.0.1:6379");
    // construct RelayContext
    auto context = std::make_shared<RelayContext>(msg_looper, redis);
    // construct RelayServer
    auto server = std::make_shared<RelayServer>(context);
    // construct RoomManager & DeviceManager
    auto room_mgr = std::make_shared<RelayRoomManager>(context, server);
    auto device_mgr = std::make_shared<RelayDeviceManager>(context, server);

    // init RelayContext
    if (!context->Init(device_mgr, room_mgr)) {
        return -1;
    }

    // init RelayServer
    server->Init();
    server->Start();

    // start MAIN message looping
    msg_looper->Loop();

    return 0;
}