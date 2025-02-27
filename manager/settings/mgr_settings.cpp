//
// Created by RGAA on 26/02/2025.
//

#include "mgr_settings.h"
#include "tc_3rdparty/toml/toml.hpp"
#include "tc_common_new/log.h"

namespace tc
{

    void MgrSettings::LoadSettings() {
        auto settings_name = "GammaRayManageServer.toml";
        toml::parse_result result;
        try {
            result = toml::parse_file(settings_name);
        } catch (std::exception& e) {
            LOGE("Parse file failed: {}", e.what());
            return;
        }
        listen_port_ = result["listen-port"].value_or(20581);
    }

    void MgrSettings::DumpSettings() {
        LOGI("---------------------Settings Beg-----------------------");
        LOGI("Listen port: {}", listen_port_);
        LOGI("---------------------Settings End-----------------------");
    }

}