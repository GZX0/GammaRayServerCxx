//
// Created by RGAA on 24/02/2025.
//

#include "mgr_context.h"
#include "mgr_server.h"
#include "tc_common_new/log.h"
#include "tc_common_new/message_looper.h"

using namespace tc;

int main(int argc, char** argv) {

    auto msg_looper = std::make_shared<MessageLooper>(-1);

    auto context = std::make_shared<MgrContext>(msg_looper);
    if (!context->Init()) {
        return -1;
    }

    auto server = std::make_shared<MgrServer>(context);
    server->Init();
    server->Start();

    msg_looper->Loop();

    return 0;
}