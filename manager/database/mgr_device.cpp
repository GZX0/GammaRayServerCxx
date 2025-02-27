//
// Created by RGAA on 26/02/2025.
//

#include "mgr_device.h"

namespace tc
{

    bsoncxx::document::value MgrDevice::AsBsonDocument() {
        return make_document(
            kvp(kMgrDeviceId, device_id_),
            kvp(kMgrDeviceBelongToUser, belong_to_user_),
            kvp(kMgrDeviceSeed, seed_),
            kvp(kMgrDeviceRandomPwd, random_pwd_),
            kvp(kMgrDeviceSafetyPwd, safety_pwd_),
            kvp(kMgrDeviceDeleted, deleted_),
            kvp(kMgrDeviceCreatedTimestamp, created_timestamp_),
            kvp(kMgrDeviceUpdatedTimestamp, updated_timestamp_),
            kvp(kMgrUsedTime, used_time_)
        );
    }

    bool MgrDevice::ParseFrom(const bsoncxx::document::value& val) {
        try {
            auto bson = val.view();
            this->obj_id_ = bson["_id"].get_oid().value.to_string();
            this->device_id_ = std::string{bson[kMgrDeviceId].get_string().value};
            this->belong_to_user_ = std::string{bson[kMgrDeviceBelongToUser].get_string().value};
            this->seed_ = std::string{bson[kMgrDeviceSeed].get_string().value};
            this->random_pwd_ = std::string{bson[kMgrDeviceRandomPwd].get_string().value};
            this->safety_pwd_ = std::string{bson[kMgrDeviceSafetyPwd].get_string().value};
            this->deleted_ = bson[kMgrDeviceDeleted].get_int32();
            this->created_timestamp_ = bson[kMgrDeviceCreatedTimestamp].get_int64();
            this->updated_timestamp_ = bson[kMgrDeviceUpdatedTimestamp].get_int64();
            this->used_time_ = bson[kMgrUsedTime].get_int64();
            return true;
        }
        catch(std::exception& e) {
            return false;
        }
    }

}