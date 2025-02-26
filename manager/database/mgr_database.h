//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_DATABASE_H
#define GAMMARAYSERVER_MGR_DATABASE_H

#include <any>
#include <memory>
#include <string>
#include <sqlite_orm/sqlite_orm.h>

namespace tc
{

    class MgrUser;
    class MgrDevice;
    class MgrContext;

    class MgrDatabase {
    public:
        explicit MgrDatabase(const std::shared_ptr<MgrContext>& ctx);
        void Init();
        std::string GenerateNewClientId(const std::string& req_info);
        auto GetStorageTypeValue();
        auto InitAppDatabase(const std::string& name);

        std::unique_ptr<MgrUser> FindUserByUid(const std::string& uid);

        void InsertDevice(std::unique_ptr<MgrDevice>&& device);
        std::unique_ptr<MgrDevice> FindDeviceByDeviceId(const std::string& device_id);
        std::unique_ptr<MgrDevice> FindDeviceByDeviceIdAndGeneratedSeed(const std::string& device_id, const std::string& seed);
        std::string GenerateRandomPassword();

    private:
        std::shared_ptr<MgrContext> context_ = nullptr;
        std::any db_storage_;
    };

}

#endif //GAMMARAYSERVER_MGR_DATABASE_H
