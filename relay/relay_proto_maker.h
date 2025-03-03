//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_PROTO_MAKER_H
#define GAMMARAYSERVER_RELAY_PROTO_MAKER_H

#include <string>
#include <vector>
#include "relay_message.pb.h"

namespace tc
{

    class RelayDevice;

    class RelayProtoMaker {
    public:

        static std::string MakeErrorMessage(const RelayErrorCode& code,
                                            const RelayMessageType& which_message);

        static std::string MakeErrorMessage(const RelayErrorCode& code,
                                            const std::string& msg,
                                            const RelayMessageType& which_message);

        static std::string MakeCreateRoomResp(const std::string& device_id,
                                              const std::string& remote_device_id,
                                              const std::string& room_id,
                                              const std::vector<std::shared_ptr<RelayDevice>>& devices);

    private:
        static std::string GetErrorMessage(const RelayErrorCode& code);
    };

}

#endif //GAMMARAYSERVER_RELAY_PROTO_MAKER_H
