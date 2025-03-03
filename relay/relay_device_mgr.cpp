//
// Created by RGAA on 25/02/2025.
//

#include "relay_device_mgr.h"
#include "relay_device.h"
#include "tc_common_new/log.h"
#include "relay_context.h"
#include "relay_server.h"
#include "relay_session.h"
#include "tc_common_new/time_ext.h"
#include <format>

namespace tc
{
    static const std::string kRsDeviceId = "device_id";
    static const std::string kRsLastUpdateTimestamp = "last_update_timestamp";
    static const std::string kRsSocketFd = "socket_fd";

    RelayDeviceManager::RelayDeviceManager(const std::shared_ptr<RelayContext>& ctx, const std::shared_ptr<RelayServer>& server) {
        context_ = ctx;
        server_ = server;
        redis_ = context_->GetRedis();
    }

    void RelayDeviceManager::AddDevice(const std::shared_ptr<RelayDevice>& device) {
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

    bool RelayDeviceManager::RemoveDevice(const std::string& device_id) {
        try {
            auto r = redis_->del(std::format("device:{}", device_id));
            return true;
        }
        catch(std::exception& e) {
            LOGE("DEL failed: {} for: {}", e.what(), device_id);
            return false;
        }
    }

    std::shared_ptr<RelayDevice> RelayDeviceManager::FindDevice(const std::string& device_id) {
        auto target_client = std::make_shared<RelayDevice>();
        auto values = std::unordered_map<std::string, std::string>();
        try {
            redis_->hgetall(std::format("device:{}", device_id), std::inserter(values, values.begin()));
            if (!values.empty()) {
                target_client->device_id_ = values[kRsDeviceId];
                target_client->last_update_timestamp_ = std::atoll(values[kRsLastUpdateTimestamp].c_str());
                target_client->socket_fd_ = std::atoll(values[kRsSocketFd].c_str());

                // find by socket fd
                if (auto sess_by_socket_fd
                        = server_->FindSessionBySocketFd<RelaySession>(target_client->socket_fd_).lock(); sess_by_socket_fd) {
                    target_client->sess_ = sess_by_socket_fd;
                }
                else if (auto sess_by_device_id
                        = server_->FindSessionByDeviceId(target_client->device_id_).lock(); sess_by_device_id) {
                    target_client->sess_ = sess_by_device_id;
                }
                else {
                    return nullptr;
                }
            }
        }
        catch(std::exception& e) {
            LOGE("HGETALL failed: {}", e.what());
        }

        return target_client;
    }

    void RelayDeviceManager::OnHeartBeat(const std::string& device_id) {
        auto id = std::format("device:{}", device_id);
        try {
            redis_->hset(id, kRsLastUpdateTimestamp, std::to_string(TimeExt::GetCurrentTimestamp()));
        }
        catch(std::exception& e) {
            LOGE("HSET failed: {}", e.what());
        }
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
    }

    std::vector<std::shared_ptr<RelayDevice>> RelayDeviceManager::FindDevices(int page, int page_size) {
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

        return {};
    }
}