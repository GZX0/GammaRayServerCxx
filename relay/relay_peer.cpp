//
// Created by RGAA on 24/02/2025.
//

#include "relay_peer.h"
#include "relay_session.h"

namespace tc
{

    void RelayPeer::Notify(const std::string& msg) const {
        if (sess_) {
            sess_->PostBinMessage(msg);
        }
    }

}