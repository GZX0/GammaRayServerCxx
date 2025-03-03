//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_SESSION_H
#define GAMMARAYSERVER_MGR_SESSION_H

#include "tc_common_new/ws_server.h"

namespace tc
{

    class MgrSession : public WsSession, public std::enable_shared_from_this<MgrSession> {
    public:
        bool OnConnected() override;
        void OnDisConnected() override;
        void OnBinMessage(std::string_view data) override;

    };

}

#endif //GAMMARAYSERVER_MGR_SESSION_H
