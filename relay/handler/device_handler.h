//
// Created by RGAA on 27/02/2025.
//

#ifndef GAMMARAYSERVER_DEVICE_HANDLER_H
#define GAMMARAYSERVER_DEVICE_HANDLER_H

#include "tc_common_new/base_handler.h"

namespace tc
{

    class RelayContext;
    class RelayServer;
    class RelayDeviceManager;

    class DeviceHandler : public BaseHandler {
    public:
        explicit DeviceHandler(const std::shared_ptr<RelayServer>& srv);
        void RegisterPaths() override;
        std::string GetErrorMessage(int code) override;

    private:
        std::shared_ptr<RelayServer> server_ = nullptr;
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayDeviceManager> client_mgr_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_DEVICE_HANDLER_H
