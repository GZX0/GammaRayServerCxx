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
        auto device = pm->FindDevice(device_id);
        if (!device|| !device->IsAlive()) {
            LOGW("This peer[My Device] is not alive: {}", device_id);
            return std::nullopt;
        }

        // check remote device
        auto remote_device = pm->FindDevice(remote_device_id);
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
        rooms_.Insert(room->room_id_, room);

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

    std::optional<std::shared_ptr<RelayRoom>> RelayRoomManager::RemoveRoom(const std::string& room_id) {
        auto r = rooms_.Remove(room_id);
        return r;
    }

    std::optional<std::weak_ptr<RelayRoom>> RelayRoomManager::FindRoom(const std::string& room_id) {
        std::weak_ptr<RelayRoom> target_room;
        rooms_.ApplyAllCond([&] (const std::string& id, const std::shared_ptr<RelayRoom>& room) {
            if (room_id == id) {
                target_room = room;
                return true;
            }
            return false;
        });
        if (target_room.lock()) {
            return target_room;
        }
        return std::nullopt;
    }

    std::optional<std::weak_ptr<RelayDevice>> RelayRoomManager::RemoveClientInRoom(const std::string& room_id, const std::string& client_id) {
        if (!rooms_.HasKey(room_id)) {
            return std::nullopt;
        }
        auto room = rooms_.Get(room_id);
        if (room->devices_.HasKey(client_id)) {
            auto client = room->devices_.Get(client_id);
            return client;
        }
        return std::nullopt;
    }

    uint32_t RelayRoomManager::GetRoomCount() {
        return rooms_.Size();
    }

    std::vector<std::weak_ptr<RelayRoom>> RelayRoomManager::FindRooms(int page, int page_size) {
        int begin = std::max(0, page - 1) * page_size;
        int end = begin + page_size;
        auto opt_rooms = rooms_.QueryRange(begin, end);
        std::vector<std::weak_ptr<RelayRoom>> target_rooms;
        if (opt_rooms.has_value()) {
            auto rooms = opt_rooms.value();
            for (const auto& r : rooms) {
                target_rooms.push_back(r);
            }
        }
        return target_rooms;
    }

    void RelayRoomManager::DestroyCreatedRoomsBy(const std::string& device_id) {
        try {
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
                auto device_mgr = context_->GetDeviceManager();
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