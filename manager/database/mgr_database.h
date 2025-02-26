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

    class MgrClient;
    class MgrContext;

    class MgrDatabase {
    public:
        explicit MgrDatabase(const std::shared_ptr<MgrContext>& ctx);
        void Init();
        std::string GenerateNewClientId(const std::string& req_info);
        auto GetStorageTypeValue();
        auto InitAppDatabase(const std::string& name);

    private:
        std::shared_ptr<MgrContext> context_ = nullptr;
        std::any db_storage_;
    };

}

#endif //GAMMARAYSERVER_MGR_DATABASE_H
