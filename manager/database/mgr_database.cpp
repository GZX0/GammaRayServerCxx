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

using namespace sqlite_orm;

namespace tc
{

    MgrDatabase::MgrDatabase(const std::shared_ptr<MgrContext>& ctx) {
        context_ = ctx;
    }

    auto MgrDatabase::InitAppDatabase(const std::string& name) {
        auto st = make_storage(name,
            make_table("user",
                make_column("id", &MgrUser::id_, primary_key().autoincrement()),
                make_column("uid", &MgrUser::uid_),
                make_column("client_id", &MgrUser::uid_),
                make_column("created_timestamp", &MgrUser::created_timestamp_),
                make_column("deleted", &MgrUser::deleted_),
                make_column("bind_qq", &MgrUser::bind_qq_),
                make_column("bind_wechat", &MgrUser::bind_wechat_),
                make_column("bind_phone", &MgrUser::bind_phone_),
                make_column("vip_type", &MgrUser::vip_type_)
            ),
            make_table("device",
                make_column("id", &MgrDevice::id_),
                make_column("device_id", &MgrDevice::device_id_),
                make_column("belong_to_user", &MgrDevice::belong_to_user_),
                make_column("seed", &MgrDevice::seed_),
                make_column("random_pwd", &MgrDevice::random_pwd_),
                make_column("safety_pwd", &MgrDevice::safety_pwd_),
                make_column("deleted", &MgrDevice::deleted_),
                make_column("created_timestamp", &MgrDevice::created_timestamp_)
           )
        );
        st.sync_schema();
        return st;
    }

    auto MgrDatabase::GetStorageTypeValue() {
        return InitAppDatabase("");
    }

    void MgrDatabase::Init() {
        auto storage = InitAppDatabase("manager.db");
        db_storage_ = storage;
        storage.sync_schema();
    }

    std::string MgrDatabase::GenerateNewClientId(const std::string& req_info) {
        std::string final_id;
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
            final_id = ss.str();
            // todo: check database
            // exist ? don't use req info anymore.
            // ignore_req_info = true;
            auto device = FindDeviceByDeviceIdAndGeneratedSeed(final_id, seed);
            if (device) {
                LOGI("Yes, found device : {}", device->device_id_);
            }
            else {
                device= FindDeviceByDeviceId(final_id);
                if (device) {
                    ignore_req_info = true;
                    LOGW("We found the same final id, but the seed is not equal, regenerate one.");
                    continue;
                }
                else {
                    // insert device
                    auto new_device = std::make_unique<MgrDevice>();
                    new_device->device_id_ = final_id;
                    new_device->seed_ = seed;
                    new_device->random_pwd_ = GenerateRandomPassword();
                    new_device->deleted_ = 0;
                    new_device->created_timestamp_ = (int64_t) TimeExt::GetCurrentTimestamp();
                    InsertDevice(std::move(new_device));
                    LOGI("Generate a new device id: {}, seed: {}", new_device->device_id_, seed);
                }
            }
            break;
        }
        return final_id;
    }

    std::unique_ptr<MgrUser> MgrDatabase::FindUserByUid(const std::string& uid) {
        return nullptr;
    }

    void MgrDatabase::InsertDevice(std::unique_ptr<MgrDevice>&& device) {
        using Storage = decltype(GetStorageTypeValue());
        auto storage = std::any_cast<Storage>(db_storage_);
        storage.insert(*device);
    }

    std::unique_ptr<MgrDevice> MgrDatabase::FindDeviceByDeviceId(const std::string& device_id) {
        using Storage = decltype(GetStorageTypeValue());
        auto storage = std::any_cast<Storage>(db_storage_);
        auto devices = storage.get_all_pointer<MgrDevice>(where(c(&MgrDevice::device_id_) == device_id));
        if (!devices.empty()) {
            auto device = std::move(devices[0]);
            devices.erase(devices.begin());
            return device;
        }
        return nullptr;
    }

    std::unique_ptr<MgrDevice> MgrDatabase::FindDeviceByDeviceIdAndGeneratedSeed(const std::string& device_id, const std::string& seed) {
        using Storage = decltype(GetStorageTypeValue());
        auto storage = std::any_cast<Storage>(db_storage_);
        auto devices = storage.get_all_pointer<MgrDevice>(
                where(c(&MgrDevice::device_id_) == device_id and c(&MgrDevice::seed_) == seed));
        if (!devices.empty()) {
            auto device = std::move(devices[0]);
            devices.erase(devices.begin());
            return device;
        }
        return nullptr;
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