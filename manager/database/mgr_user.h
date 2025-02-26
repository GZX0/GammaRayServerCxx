//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_USER_H
#define GAMMARAYSERVER_MGR_USER_H

#include <string>

namespace tc
{

    class MgrUser {
    public:
        // database id
        int id_{};
        // user id
        std::string uid_;
        // created time
        int64_t created_timestamp_{};
        // 0 -> alive
        // 1-> deleted
        int deleted_{0};
        // binding qq
        std::string bind_qq_;
        // binding wechat
        std::string bind_wechat_;
        // binding phone
        std::string bind_phone_;
        // vip type
        std::string vip_type_;
    };

}

#endif //GAMMARAYSERVER_MGR_USER_H
