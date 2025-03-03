//
// Created by RGAA on 25/02/2025.
//

#include "relay_device_mgr.h"
#include "relay_device.h"
#include "tc_common_new/log.h"
#include "relay_context.h"
#include <format>

namespace tc
{
    static const std::string kRsDeviceId = "device_id";
    static const std::string kRsLastUpdateTimestamp = "last_update_timestamp";
    static const std::string kRsSocketFd = "socket_fd";

    RelayDeviceManager::RelayDeviceManager(const std::shared_ptr<RelayContext>& ctx) {
        context_ = ctx;
        redis_ = context_->GetRedis();
    }

    void RelayDeviceManager::AddDevice(const std::shared_ptr<RelayDevice>& device) {
        devices_.Insert(device->device_id_, device);

        auto id = std::format("device:{}", device->device_id_);
        auto values = std::unordered_map<std::string, std::string>{
            {kRsDeviceId, device->device_id_},
            {kRsLastUpdateTimestamp, std::to_string(device->last_update_timestamp_)},
            {kRsSocketFd, std::to_string(device->socket_fd_)},
        };
        try {
            redis_->hmset(id, values.begin(), values.end());
        }
        catch(std::exception& e) {
            LOGE("HMSET failed: {}", e.what());
        }
    }

    std::optional<std::shared_ptr<RelayDevice>> RelayDeviceManager::RemoveDevice(const std::string& device_id) {
        try {
            redis_->del(std::format("device:{}", device_id));
        }
        catch(std::exception& e) {
            LOGE("DEL failed: {} for: {}", e.what(), device_id);
        }

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

        auto values = std::unordered_map<std::string, std::string>();
        try {
            redis_->hgetall(std::format("device:{}", device_id), std::inserter(values, values.begin()));
            if (!values.empty()) {
                //todo:
            }
        }
        catch(std::exception& e) {
            LOGE("HGETALL failed: {}", e.what());
        }

        return target_client;
    }

    uint32_t RelayDeviceManager::GetClientCount() {
        std::vector<std::string> keys;
        try {
            redis_->keys("device:*", std::back_inserter(keys));
        }
        catch(std::exception& e) {
            LOGE("KEYS failed: {}", e.what());
            return 0;
        }
        return keys.size();

        //return devices_.Size();
    }

    std::vector<std::weak_ptr<RelayDevice>> RelayDeviceManager::FindDevices(int page, int page_size) {
        int begin = std::max(0, page - 1) * page_size;
        int end = begin + page_size;

        sw::redis::Cursor cursor = begin;
        auto pattern = "device:*";
        auto count = page_size;
        std::unordered_set<std::string> keys;
        while (true) {
            cursor = redis_->scan(cursor, pattern, count, std::inserter(keys, keys.begin()));
            if (cursor == 0) {
                break;
            }
        }
        for (const auto& k : keys) {
            LOGI("==> key: {}", k);
        }

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