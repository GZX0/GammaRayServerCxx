//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_MGR_CLIENT_H
#define GAMMARAYSERVER_MGR_CLIENT_H

#include <string>

namespace tc
{

    class MgrClient {
    public:
        int id_{};
        std::string client_id_;
        int64_t created_timestamp_{};
    };

}

#endif //GAMMARAYSERVER_MGR_CLIENT_H
