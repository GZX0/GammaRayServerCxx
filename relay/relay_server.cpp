//
// Created by RGAA on 24/02/2025.
//

#include "relay_server.h"
#include "tc_common_new/log.h"

namespace tc
{

    struct aop_log {
        bool before(http::web_request &req, http::web_response &rep) {
            asio2::ignore_unused(rep);
            return true;
        }

        bool after(std::shared_ptr<asio2::http_session> &session_ptr, http::web_request &req, http::web_response &rep) {
                    ASIO2_ASSERT(asio2::get_current_caller<std::shared_ptr<asio2::http_session>>().get() == session_ptr.get());
            asio2::ignore_unused(session_ptr, req, rep);
            return true;
        }
    };

    template<typename Server>
    void RelayServer::AddWebsocketRouter(const std::string &path, const Server &s) {
        auto fn_get_socket_fd = [](std::shared_ptr<asio2::http_session> &sess_ptr) -> uint64_t {
            auto& s = sess_ptr->socket();
            return (uint64_t)s.native_handle();
        };
        s->bind(path, websocket::listener<asio2::http_session>{}
                .on("message", [=, this](std::shared_ptr<asio2::http_session> &sess_ptr, std::string_view data) {
                    auto socket_fd = fn_get_socket_fd(sess_ptr);
//                    if (path == kUrlPanel) {
//                        this->ParsePanelBinaryMessage(socket_fd, data);
//                    }
//                    else if (path == kUrlFileTransfer) {
//                        this->ParseFtBinaryMessage(socket_fd, data);
//                    }
                })
                .on("open", [=, this](std::shared_ptr<asio2::http_session> &sess_ptr) {
                    LOGI("App server {} open", path);
                    sess_ptr->ws_stream().binary(true);
                    sess_ptr->set_no_delay(true);
                    auto socket_fd = fn_get_socket_fd(sess_ptr);
//                    if (path == kUrlPanel) {
//                        auto ws_sess = std::make_shared<WSSession>();
//                        ws_sess->socket_fd_ = socket_fd;
//                        ws_sess->session_ = sess_ptr;
//                        this->panel_sessions_.Insert(socket_fd, ws_sess);
//                        LOGI("client connect : {}", socket_fd);
//
//                        //this->NotifyPeerConnected();
//                    }
//                    else if (path == kUrlFileTransfer) {
//                        auto ft_sess = std::make_shared<FtSession>();
//                        ft_sess->socket_fd_ = socket_fd;
//                        ft_sess->session_ = sess_ptr;
//                        ft_sess->ch_ = std::make_shared<FileTransferChannel>(context_, sess_ptr);
//                        this->ft_sessions_.Insert(socket_fd, ft_sess);
//                        ft_sess->ch_->OnConnected();
//                    }
                })
                .on("close", [=, this](std::shared_ptr<asio2::http_session> &sess_ptr) {
                    auto socket_fd = fn_get_socket_fd(sess_ptr);
//                    if (path == kUrlPanel) {
//                        if (panel_sessions_.HasKey(socket_fd)) {
//                            panel_sessions_.Remove(socket_fd);
//                        }
//                        //this->NotifyPeerDisconnected();
//                    }
//                    else if (path == kUrlFileTransfer) {
//                        if (ft_sessions_.HasKey(socket_fd)) {
//                            auto ft_session = ft_sessions_.Get(socket_fd);
//                            ft_session->ch_->OnDisConnected();
//                            ft_sessions_.Remove(socket_fd);
//                        }
//                    }
                })
                .on_ping([=, this](auto &sess_ptr) {

                })
                .on_pong([=, this](auto &sess_ptr) {

                })
        );
    }

    void RelayServer::AddHttpGetRouter(const std::string &path,
                                         std::function<void(const std::string& path, http::web_request &req, http::web_response &rep)>&& cbk) {
        http_server_->bind<http::verb::get>(path, [=, this](http::web_request &req, http::web_response &rep) {
            cbk(path, req, rep);
        }, aop_log{});
    }

    void RelayServer::AddHttpPostRouter(const std::string& path,
                                          std::function<void(const std::string& path, http::web_request &req, http::web_response &rep)>&& cbk) {
        http_server_->bind<http::verb::post>(path, [=, this](http::web_request &req, http::web_response &rep) {
            cbk(path, req, rep);
        }, aop_log{});
    }
    
}