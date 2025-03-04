//
// Created by RGAA on 25/02/2025.
//

#include <format>
#include "relay_room_mgr.h"
#include "relay_room.h"
#include "relay_device_mgr.h"
#include "relay_context.h"
#include "relay_device.h"
#include "tc_common_new/log.h"
#include "tc_common_new/time_ext.h"

namespace tc
{

    static const std::string kRsRoomId = "room_id";
    static const std::string kRsRoomDeviceId = "device_id";
    static const std::string kRsRoomRemoteDeviceId = "remote_device_id";
    static const std::string kRsRoomLastUpdateTimestamp = "last_update_timestamp";

    RelayRoomManager::RelayRoomManager(const std::shared_ptr<RelayContext>& ctx, const std::shared_ptr<RelayServer>& server) {
        context_ = ctx;
        redis_ = context_->GetRedis();
        server_ = server;
    }

    std::optional<std::shared_ptr<RelayRoom>> RelayRoomManager::CreateRoom(const std::string& device_id, const std::string& remote_device_id) {
        auto pm = context_->GetDeviceManager();
        if (!pm) {
            return std::nullopt;
        }
        // check myself
        auto device = pm->FindDeviceById(device_id);
        if (!device|| !device->IsAlive()) {
            LOGW("This peer[My Device] is not alive: {}", device_id);
            return std::nullopt;
        }

        // check remote device
        auto remote_device = pm->FindDeviceById(remote_device_id);
        if (!remote_device || !remote_device->IsAlive()) {
            LOGW("This peer[Remote Client] is not alive: {}", remote_device_id);
            return std::nullopt;
        }

        auto room_id = std::format("room:{}-{}", device_id, remote_device_id);
        auto room = std::make_shared<RelayRoom>();
        room->room_id_ = room_id;
        room->device_id_ = device_id;
        room->remote_device_id_ = remote_device_id;
        room->context_ = context_;
        room->devices_.Insert(device_id, device);
        room->devices_.Insert(remote_device_id, remote_device);

        // insert to redis
        try {
            std::unordered_map<std::string, std::string> values = {
                {kRsRoomId, room_id},
                {kRsRoomDeviceId, device_id},
                {kRsRoomRemoteDeviceId, remote_device_id},
                {kRsRoomLastUpdateTimestamp, std::to_string(TimeExt::GetCurrentTimestamp())},
            };
            redis_->hmset(room_id, values.begin(), values.end());
        }
        catch(std::exception& e) {
            LOGE("HMSET failed for CreateRoom: {}", e.what());
        }

        return room;
    }

    bool RelayRoomManager::RemoveRoom(const std::string& room_id) {
        try {
            auto r = redis_->del(room_id);
            return r > 0;
        }
        catch(std::exception& e) {
            LOGE("DEL failed: {} for: {}", e.what(), room_id);
            return false;
        }
    }

    std::shared_ptr<RelayRoom> RelayRoomManager::FindRoom(const std::string& room_id) {
        try {
            auto values = std::unordered_map<std::string, std::string>();
            redis_->hgetall(room_id, std::inserter(values, values.begin()));
            if (values.empty()) {
                return nullptr;
            }
            auto room = std::make_shared<RelayRoom>();
            room->room_id_ = values[kRsRoomId];
            room->device_id_ = values[kRsRoomDeviceId];
            room->remote_device_id_ = values[kRsRoomRemoteDeviceId];
            room->last_update_timestamp_ = std::atoll(values[kRsRoomLastUpdateTimestamp].c_str());
            auto device_mgr = context_->GetDeviceManager();
            if (auto device = device_mgr->FindDeviceById(room->device_id_); device) {
                room->devices_.Insert(room->device_id_, device);
            }
            if (auto remote_device = device_mgr->FindDeviceById(room->remote_device_id_); remote_device) {
                room->devices_.Insert(room->remote_device_id_, remote_device);
            }
            return room;
        }
        catch (std::exception& e) {
            LOGE("HGETALL failed: {} for room id: {}", e.what(), room_id);
        }

        return nullptr;
    }

    bool RelayRoomManager::RemoveClientInRoom(const std::string& room_id, const std::string& client_id) {
        return false;
    }

    uint32_t RelayRoomManager::GetRoomCount() {
        std::vector<std::string> keys;
        try {
            redis_->keys("room:*", std::back_inserter(keys));
        }
        catch(std::exception& e) {
            LOGE("KEYS failed: {}", e.what());
            return 0;
        }
        return keys.size();
    }

    std::vector<std::shared_ptr<RelayRoom>> RelayRoomManager::FindRooms(int page, int page_size) {
        int begin = std::max(0, page - 1) * page_size;
        int end = begin + page_size;

        sw::redis::Cursor cursor = begin;
        auto pattern = "room:*";
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

    void RelayRoomManager::DestroyCreatedRoomsBy(const std::string& device_id) {
        try {
            auto device_mgr = context_->GetDeviceManager();
            std::vector<std::string> room_ids;
            redis_->keys(std::format("room:{}*", device_id), std::back_inserter(room_ids));
            if (room_ids.empty()) {
                return;
            }
            for (const auto& room_id : room_ids) {
                std::unordered_map<std::string, std::string> values;
                redis_->hgetall(room_id, std::inserter(values, values.begin()));
                if (values.empty()) {
                    continue;
                }

                // get remote device id and notify the device
                std::string remote_device_id;
                for (const auto& [key, val] : values) {
                    if (key == kRsRoomRemoteDeviceId) {
                        remote_device_id = val;
                        break;
                    }
                }
                device_mgr->NotifyDeviceRoomDestroyed(device_id, remote_device_id, room_id);

                // delete in redis
                redis_->del(room_id);
            }
        }
        catch(std::exception& e) {
            LOGE("OnHeartBeatForMyRoom failed: {}, id: {}", e.what(), device_id);
        }
    }

    void RelayRoomManager::OnHeartBeatForMyRoom(const std::string& device_id) {
        try {
            std::vector<std::string> room_ids;
            redis_->keys(std::format("room:{}*", device_id), std::back_inserter(room_ids));
            if (room_ids.empty()) {
                return;
            }
            for (const auto& room_id : room_ids) {
                redis_->hset(room_id, kRsRoomLastUpdateTimestamp, std::to_string(TimeExt::GetCurrentTimestamp()));
            }
        }
        catch(std::exception& e) {
            LOGE("OnHeartBeatForMyRoom failed: {}, id: {}", e.what(), device_id);
        }
    }

}