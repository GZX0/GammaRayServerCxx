//
// Created by RGAA on 25/02/2025.
//

#include "relay_device_mgr.h"
#include "relay_device.h"
#include "tc_common_new/log.h"
#include <format>

namespace tc
{

    RelayDeviceManager::RelayDeviceManager(const std::shared_ptr<RelayContext>& ctx) {
        context_ = ctx;
    }

    void RelayDeviceManager::AddClient(const std::shared_ptr<RelayDevice>& client) {
        devices_.Insert(client->device_id_, client);
    }

    std::optional<std::shared_ptr<RelayDevice>> RelayDeviceManager::RemoveClient(const std::string& device_id) {
        return devices_.Remove(device_id);
    }

    std::weak_ptr<RelayDevice> RelayDeviceManager::FindDevice(const std::string& device_id) {
        std::weak_ptr<RelayDevice> target_client;
        devices_.ApplyAllCond([&](const std::string& cid, const std::shared_ptr<RelayDevice>& client) {
            if (cid == device_id) {
                target_client = client;
                return true;
            }
            return false;
        });
        return target_client;
    }

    uint32_t RelayDeviceManager::GetClientCount() {
        return devices_.Size();
    }

    std::vector<std::weak_ptr<RelayDevice>> RelayDeviceManager::FindDevices(int page, int page_size) {
        int begin = std::max(0, page - 1) * page_size;
        int end = begin + page_size;
        auto opt_clients = devices_.QueryRange(begin, end);
        std::vector<std::weak_ptr<RelayDevice>> target_clients;
        if (opt_clients.has_value()) {
            auto clients = opt_clients.value();
            for (const auto& c : clients) {
                target_clients.push_back(c);
            }
        }
        return target_clients;
    }
}