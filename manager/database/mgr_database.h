//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_DATABASE_H
#define GAMMARAYSERVER_MGR_DATABASE_H

#include <any>
#include <memory>
#include <string>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

namespace tc
{

    class MgrUser;
    class MgrDevice;
    class MgrContext;

    class MgrDatabase {
    public:
        explicit MgrDatabase(const std::shared_ptr<MgrContext>& ctx);
        bool Init(const std::string& url, const std::string& db_name);

        std::shared_ptr<MgrUser> FindUserByUid(const std::string& uid);

        std::shared_ptr<MgrDevice> GenerateNewDevice(const std::string& req_info, const std::string& platform);
        bool InsertDevice(const std::shared_ptr<MgrDevice>& device);
        bool UpdateDevice(const std::string& device_id, const std::map<std::string, std::string>& info);
        std::shared_ptr<MgrDevice> FindDeviceByDeviceId(const std::string& device_id);
        std::shared_ptr<MgrDevice> FindDeviceByDeviceIdAndGeneratedSeed(const std::string& device_id, const std::string& seed);
        static std::string GenerateRandomPassword();

    private:
        std::shared_ptr<MgrContext> context_ = nullptr;

        std::shared_ptr<mongocxx::instance> mgo_instance_ = nullptr;
        std::shared_ptr<mongocxx::client> mgo_client_ = nullptr;
        mongocxx::database mgo_db_;
        mongocxx::collection c_device_;
        mongocxx::collection c_user_;
    };

}

#endif //GAMMARAYSERVER_MGR_DATABASE_H
