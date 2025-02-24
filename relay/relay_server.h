//
// Created by RGAA on 24/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_SERVER_H
#define GAMMARAYSERVER_RELAY_SERVER_H

#include "tc_common_new/concurrent_hashmap.h"
#include <asio2/asio2.hpp>

namespace tc
{

    class WsData {
    public:
        std::map<std::string, std::any> vars_;
    };
    using WsDataPtr = std::shared_ptr<WsData>;

    class WSSession {
    public:
        uint64_t socket_fd_;
        int session_type_;
        std::shared_ptr<asio2::http_session> session_ = nullptr;
    };

    class RelayServer {
    public:

    private:
        template<typename Server>
        void AddWebsocketRouter(const std::string& path, const Server& s);

        void AddHttpGetRouter(const std::string& path,
                              std::function<void(const std::string& path, http::web_request &req, http::web_response &rep)>&& cbk);

        void AddHttpPostRouter(const std::string& path,
                               std::function<void(const std::string& path, http::web_request &req, http::web_response &rep)>&& cbk);

    private:
        std::shared_ptr<asio2::http_server> http_server_ = nullptr;
        ConcurrentHashMap<uint64_t, std::shared_ptr<WSSession>> panel_sessions_;
    };

}

#endif //GAMMARAYSERVER_RELAY_SERVER_H
