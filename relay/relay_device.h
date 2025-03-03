//
// Created by RGAA on 24/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_DEVICE_H
#define GAMMARAYSERVER_RELAY_DEVICE_H

#include <string>
#include <memory>

namespace tc
{

    class RelaySession;

    class RelayDevice {
    public:
        void Notify(const std::string& msg) const;
        [[nodiscard]] bool IsAlive() const;

    public:
        std::string device_id_;
        int64_t last_update_timestamp_ = 0;
        int64_t socket_fd_{0};
        // todo: weak ptr here, all in RelayServer
        std::shared_ptr<RelaySession> sess_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_RELAY_DEVICE_H
