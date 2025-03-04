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
#include "relay_message.pb.h"

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

    bool RelayDeviceManager::AddDevice(const std::shared_ptr<RelayDevice>& device) {
        auto id = std::format("device:{}", device->device_id_);
        auto values = std::unordered_map<std::string, std::string>{
            {kRsDeviceId, device->device_id_},
            {kRsLastUpdateTimestamp, std::to_string(device->last_update_timestamp_)},
            {kRsSocketFd, std::to_string(device->socket_fd_)},
        };
        try {
            redis_->hmset(id, values.begin(), values.end());
            return true;
        }
        catch(std::exception& e) {
            LOGE("HMSET failed: {}", e.what());
            return false;
        }
    }

    bool RelayDeviceManager::RemoveDeviceById(const std::string& device_id) {
        return this->RemoveDeviceByRedisKey(std::format("device:{}", device_id));
    }

    bool RelayDeviceManager::RemoveDeviceByRedisKey(const std::string& key) {
        try {
            auto r = redis_->del(key);
            return r > 0;
        }
        catch(std::exception& e) {
            LOGE("DEL failed: {} for: {}", e.what(), key);
            return false;
        }
    }

    std::shared_ptr<RelayDevice> RelayDeviceManager::FindDeviceById(const std::string& device_id) {
        return this->FindDeviceByRedisKey(std::format("device:{}", device_id));
    }

    std::shared_ptr<RelayDevice> RelayDeviceManager::FindDeviceByRedisKey(const std::string& key) {
        auto target_client = std::make_shared<RelayDevice>();
        auto values = std::unordered_map<std::string, std::string>();
        try {
            redis_->hgetall(key, std::inserter(values, values.begin()));
            if (values.empty()) {
                return nullptr;
            }
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
        catch(std::exception& e) {
            LOGE("HGETALL failed: {}", e.what());
        }

        return target_client;
    }

    void RelayDeviceManager::OnHeartBeat(int64_t socket_fd, const std::string& device_id) {
        auto id = std::format("device:{}", device_id);
        try {
            std::unordered_map<std::string, std::string> values = {
                {kRsDeviceId, device_id},
                {kRsSocketFd, std::to_string(socket_fd)},
                {kRsLastUpdateTimestamp, std::to_string(TimeExt::GetCurrentTimestamp())}
            };
            redis_->hmset(id, values.begin(), values.end());
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
        cursor = redis_->scan(cursor, pattern, count, std::inserter(keys, keys.begin()));

        LOGI("===> Total keys: {}", keys.size());
        std::vector<std::shared_ptr<RelayDevice>> devices;
        for (const auto& device_key : keys) {
            LOGI("==> key: {}", device_key);
            auto device = FindDeviceByRedisKey(device_key);
            if (!device) {
                LOGW("This device may not alive anymore: {}", device_key);
                continue;
            }
            devices.push_back(device);
        }

        return devices;
    }

    void RelayDeviceManager::NotifyDeviceRoomDestroyed(const std::string& device_id, const std::string& remote_device_id, const std::string& room_id) {
        // device id: this device has left.
        // remote device id: this device will be notified
        auto remote_device = FindDeviceById(remote_device_id);
        if (!remote_device) {
            LOGW("Notify device: {} room: {} destroyed, but the device does not exist");
            return;
        }

        RelayMessage rl_msg;
        rl_msg.set_type(RelayMessageType::kRelayRoomDestroyed);
        auto sub = rl_msg.mutable_room_destroyed();
        sub->set_room_id(room_id);
        sub->set_device_id(device_id);
        sub->set_remote_device_id(remote_device_id);
        remote_device->Notify(rl_msg.SerializeAsString());
    }
}