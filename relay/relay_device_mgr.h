//
// Created by RGAA on 25/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_DEVICE_MGR_H
#define GAMMARAYSERVER_RELAY_DEVICE_MGR_H

#include <memory>
#include "tc_common_new/concurrent_hashmap.h"
#include <sw/redis++/redis++.h>

using namespace sw::redis;

namespace tc
{
    class RelayDevice;
    class RelayContext;

    class RelayDeviceManager {
    public:
        explicit RelayDeviceManager(const std::shared_ptr<RelayContext>& ctx);
        //
        void AddDevice(const std::shared_ptr<RelayDevice>& device);
        //
        std::optional<std::shared_ptr<RelayDevice>> RemoveDevice(const std::string& device_id);
        //
        std::weak_ptr<RelayDevice> FindDevice(const std::string& device_id);

        // total count
        uint32_t GetClientCount();

        std::vector<std::weak_ptr<RelayDevice>> FindDevices(int page, int page_size);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<Redis> redis_ = nullptr;
        ConcurrentHashMap<std::string, std::shared_ptr<RelayDevice>> devices_;
    };
}

#endif //GAMMARAYSERVER_RELAY_DEVICE_MGR_H
