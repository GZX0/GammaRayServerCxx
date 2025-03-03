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
    class RelayServer;

    class RelayDeviceManager {
    public:
        explicit RelayDeviceManager(const std::shared_ptr<RelayContext>& ctx, const std::shared_ptr<RelayServer>& server);
        //
        void AddDevice(const std::shared_ptr<RelayDevice>& device);
        //
        bool RemoveDevice(const std::string& device_id);
        //
        std::shared_ptr<RelayDevice> FindDevice(const std::string& device_id);
        //
        void OnHeartBeat(const std::string& device_id);

        // total count
        uint32_t GetClientCount();

        std::vector<std::shared_ptr<RelayDevice>> FindDevices(int page, int page_size);

        // device_id: room creator
        // remote_device_id: will be notified
        // room_id: room's id
        void NotifyDeviceRoomDestroyed(const std::string& device_id, const std::string& remote_device_id, const std::string& room_id);

    private:
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayServer> server_ = nullptr;
        std::shared_ptr<Redis> redis_ = nullptr;
        //ConcurrentHashMap<std::string, std::shared_ptr<RelayDevice>> devices_;
    };
}

#endif //GAMMARAYSERVER_RELAY_DEVICE_MGR_H
