//
// Created by RGAA on 26/02/2025.
//

#include "mgr_database.h"
#include "tc_common_new/md5.h"
#include "tc_common_new/uuid.h"
#include "tc_common_new/log.h"
#include "mgr_client.h"

using namespace sqlite_orm;

namespace tc
{

    MgrDatabase::MgrDatabase(const std::shared_ptr<MgrContext>& ctx) {
        context_ = ctx;
    }

    auto MgrDatabase::InitAppDatabase(const std::string& name) {
        auto st = make_storage(name,
            make_table("games",
                make_column("id", &MgrClient::id_, primary_key().autoincrement()),
                make_column("client_id", &MgrClient::client_id_),
                make_column("created_timestamp", &MgrClient::created_timestamp_)
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
            LOGI("Seed: {}", seed);
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
            break;
        }
        return final_id;
    }

}