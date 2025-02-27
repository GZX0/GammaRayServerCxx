//
// Created by RGAA on 27/02/2025.
//

#ifndef GAMMARAYSERVER_ROOM_HANDLER_H
#define GAMMARAYSERVER_ROOM_HANDLER_H

#include "tc_common_new/base_handler.h"

namespace tc
{

    class RelayContext;
    class RelayServer;
    class RelayRoomManager;

    class RoomHandler : public BaseHandler {
    public:
        explicit RoomHandler(const std::shared_ptr<RelayServer>& srv);
        void RegisterPaths() override;
        std::string GetErrorMessage(int code) override;

    private:
        std::shared_ptr<RelayServer> server_ = nullptr;
        std::shared_ptr<RelayContext> context_ = nullptr;
        std::shared_ptr<RelayRoomManager> room_mgr_ = nullptr;
    };

}

#endif //GAMMARAYSERVER_ROOM_HANDLER_H
