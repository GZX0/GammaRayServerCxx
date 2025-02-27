//
// Created by RGAA on 26/02/2025.
//

#include "mgr_database.h"
#include "tc_common_new/md5.h"
#include "tc_common_new/uuid.h"
#include "tc_common_new/log.h"
#include "tc_common_new/time_ext.h"
#include "mgr_user.h"
#include "mgr_device.h"

#include <iostream>
#include <random>
#include <ctime>

namespace tc
{

    MgrDatabase::MgrDatabase(const std::shared_ptr<MgrContext>& ctx) {
        context_ = ctx;
    }

    bool MgrDatabase::Init(const std::string& url, const std::string& db_name) {
        try {
            mgo_instance_ = std::make_shared<mongocxx::instance>();
            mongocxx::uri uri(url);
            mgo_client_ = std::make_shared<mongocxx::client>(uri);
            mgo_db_ = mgo_client_->database(db_name);
            c_device_ = mgo_db_.collection("gr_device");
            c_user_ = mgo_db_.collection("gr_user");
        } catch(std::exception& e) {
            LOGE("mongodb init failed: {} {}", url, db_name);
            return false;
        }
        return true;
    }

    std::shared_ptr<MgrDevice> MgrDatabase::GenerateNewDevice(const std::string& req_info, const std::string& platform) {
        std::shared_ptr<MgrDevice> the_device = nullptr;
        bool ignore_req_info = false;
        while (true) {
            std::string seed;
            if (!req_info.empty() && !ignore_req_info) {
                seed = req_info;
            } else {
                seed = GetUUID();
            }

            auto md5_str = MD5::Hex(seed);
            std::stringstream ss;
            ss
                << md5_str[0] % 10
                << md5_str[7] % 10
                << md5_str[11] % 10
                << md5_str[16] % 10
                << md5_str[18] % 10
                << md5_str[23] % 10
                << md5_str[26] % 10
                << md5_str[28] % 10
                << md5_str[30] % 10;
            auto new_client_id = ss.str();
            auto device = FindDeviceByDeviceIdAndGeneratedSeed(new_client_id, seed);
            if (device) {
                auto new_random_pwd = GenerateRandomPassword();
                // update new random password
                LOGI("Before update device.");
                device->random_pwd_ = MD5::Hex(new_random_pwd);
                this->UpdateDevice(device->device_id_, {
                    {kMgrDeviceRandomPwd, device->random_pwd_}
                });
                LOGI("After update device.");
                // recover to clear pwd, then -> client
                the_device = std::move(device);
                the_device->random_pwd_ = new_random_pwd;
            }
            else {
                device= FindDeviceByDeviceId(new_client_id);
                if (device) {
                    ignore_req_info = true;
                    LOGW("We found the same final id, but the seed is not equal, regenerate one.");
                    continue;
                }
                else {
                    // insert device
                    auto new_random_pwd = GenerateRandomPassword();
                    auto new_device = std::make_shared<MgrDevice>();
                    new_device->device_id_ = new_client_id;
                    new_device->seed_ = seed;
                    new_device->random_pwd_ = MD5::Hex(new_random_pwd);
                    new_device->deleted_ = 0;
                    new_device->created_timestamp_ = (int64_t) TimeExt::GetCurrentTimestamp();
                    new_device->updated_timestamp_ = (int64_t) TimeExt::GetCurrentTimestamp();
                    new_device->platform_ = platform;
                    InsertDevice(new_device);

                    new_device->random_pwd_ = new_random_pwd;
                    the_device = new_device;
                    LOGI("Generate a new device id: {}, seed: {}", the_device->device_id_, seed);
                }
            }
            break;
        }
        return the_device;
    }

    std::shared_ptr<MgrUser> MgrDatabase::FindUserByUid(const std::string& uid) {
        return nullptr;
    }

    bool MgrDatabase::InsertDevice(const std::shared_ptr<MgrDevice>& device) {
        auto doc = device->AsBsonDocument();
        try {
            c_device_.insert_one(doc.view());
            return true;
        } catch(std::exception& e) {
            LOGE("Insert failed:{}", e.what());
            return false;
        }
    }

    bool MgrDatabase::UpdateDevice(const std::string& device_id, const std::map<std::string, std::string>& info) {
        auto update_doc = bsoncxx::builder::basic::document{};
        for (const auto& [k, v] : info) {
            update_doc.append(kvp(k, v));
        }
        update_doc.append(kvp(kMgrDeviceUpdatedTimestamp, (int64_t)(TimeExt::GetCurrentTimestamp())));

        auto r = c_device_.update_one(
            make_document(kvp(kMgrDeviceId, device_id)),
            make_document(kvp("$set", update_doc.view()))
        );

        return r.has_value();
    }

    std::shared_ptr<MgrDevice> MgrDatabase::FindDeviceByDeviceId(const std::string& device_id) {
        auto result = c_device_.find_one(make_document(
            kvp(kMgrDeviceId, device_id)
        ));
        if (!result.has_value()) {
            return nullptr;
        }
        auto val = result.value();
        auto device = std::make_shared<MgrDevice>();
        device->ParseFrom(val);
        return device;
    }

    std::shared_ptr<MgrDevice> MgrDatabase::FindDeviceByDeviceIdAndGeneratedSeed(const std::string& device_id, const std::string& seed) {
        auto result = c_device_.find_one(make_document(
            kvp(kMgrDeviceId, device_id),
            kvp(kMgrDeviceSeed, seed)
        ));
        if (!result.has_value()) {
            return nullptr;
        }
        auto val = result.value();
        auto device = std::make_shared<MgrDevice>();
        device->ParseFrom(val);
        return device;
    }

    std::string MgrDatabase::GenerateRandomPassword() {
        std::string random_string;
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> distribution(0, 61);
        for (int i = 0; i < 8; ++i) {
            int random_num = distribution(rng);
            char random_char;
            if (random_num < 26) {
                random_char = 'A' + random_num;
            } else if (random_num < 52) {
                random_char = 'a' + random_num - 26;
            } else {
                random_char = '0' + random_num - 52;
            }
            random_string.push_back(random_char);
        }
        return random_string;
    }

}