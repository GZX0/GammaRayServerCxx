//
// Created by RGAA on 24/02/2025.
//

#include "relay_client.h"
#include "relay_session.h"
#include "tc_common_new/time_ext.h"

namespace tc
{

    void RelayClient::Notify(const std::string& msg) const {
        if (sess_) {
            sess_->PostBinMessage(msg);
        }
    }

    bool RelayClient::IsAlive() const {
        auto diff = last_update_timestamp_ - (int64_t)TimeExt::GetCurrentTimestamp();
        return diff < 1000 * 60;
    }

}