//
// Created by RGAA on 24/02/2025.
//

#include "relay_device.h"
#include "relay_session.h"
#include "tc_common_new/time_ext.h"

namespace tc
{

    void RelayDevice::Notify(const std::string& msg) const {
        if (auto sess = sess_.lock(); sess) {
            sess->PostBinMessage(msg);
        }
        else {
            LOGE("No session for id: {}", this->device_id_);
        }
    }

    bool RelayDevice::IsAlive() const {
        auto diff = (int64_t)TimeExt::GetCurrentTimestamp() - last_update_timestamp_ ;
        return diff < 1000 * 60;
    }

}