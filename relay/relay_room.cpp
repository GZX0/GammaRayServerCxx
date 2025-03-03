//
// Created by RGAA on 25/02/2025.
//

#include "relay_room.h"
#include "relay_device.h"

namespace tc
{
    //      client 1
    // -->  client 2
    //      client 3
    void RelayRoom::NotifyAll(const std::string& msg) {
        devices_.ApplyAll([&](const std::string& id, const std::shared_ptr<RelayDevice>& client) {
            auto target_client = client;
            if (target_client) {
                target_client->Notify(msg);
            }
        });
    }

    // --> client[client id]
    void RelayRoom::NotifyTarget(const std::string& device_id, const std::string& msg) {
        devices_.ApplyAllCond([&](const std::string& id, const std::shared_ptr<RelayDevice>& client) {
            if (id == device_id) {
                auto target_client = client;
                if (target_client) {
                    target_client->Notify(msg);
                }
                return true;
            }
            return false;
        });
    }

    //      xxx client 1 [client id]
    // -->  client 2
    //      client 3
    void RelayRoom::NotifyExcept(const std::string& device_id, const std::string& msg) {
        //LOGI("device size in room : {}", devices_.Size());
        devices_.ApplyAll([&](const std::string& id, const std::shared_ptr<RelayDevice>& device) {
            if (id == device_id) {
                return;
            }
            if (device) {
                //LOGI("Notify for : {}", device->device_id_);
                device->Notify(msg);
            }
        });
    }

    std::vector<std::shared_ptr<RelayDevice>> RelayRoom::GetDevices() {
        std::vector<std::shared_ptr<RelayDevice>> clients;
        devices_.ApplyAll([&](const auto& k, const auto& client) {
            clients.push_back(client);
        });
        return clients;
    }

    void RelayRoom::AddDevice(const std::shared_ptr<RelayDevice>& dev) {
        devices_.Insert(dev->device_id_, dev);
    }

    std::optional<std::shared_ptr<RelayDevice>> RelayRoom::RemoveDevice(const std::string& device_id) {
        return devices_.Remove(device_id);
    }

}