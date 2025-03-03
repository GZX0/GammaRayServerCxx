//
// Created by RGAA on 26/02/2025.
//

#include "mgr_session.h"

namespace tc
{

    bool MgrSession::OnConnected() {
        return true;
    }

    void MgrSession::OnDisConnected() {

    }

    void MgrSession::OnBinMessage(std::string_view data) {

    }

}