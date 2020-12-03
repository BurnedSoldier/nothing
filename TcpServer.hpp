#pragma once

#include "Acceptor.hpp"
#include "TcpConnection.hpp"

#include <string>

class TcpServer : noncopyable {
public:
  TcpServer(EventLoop *loop, uint16_t port)
      : loop_(loop),
        acceptor_(loop_, port, std::bind(&TcpServer::newConnection, this, _1)) {
  }

  void start(const OnConnection &connCb, const OnMessageCb &mgsCb) {
    connCb_ = connCb;
    mgsCb_ = mgsCb;
  }

private:
  void newConnection(int connfd) {
    auto connptr = std::make_shared<TcpConnection>(loop_, connfd, mgsCb_);
    userMap_.insert({connfd, connptr});
    connCb_(connptr);
  }

private:
  EventLoop *loop_;
  Acceptor acceptor_;
  std::map<int, std::shared_ptr<TcpConnection>> userMap_;

  struct epoll_event eventNum[500];

  OnConnection connCb_;
  OnMessageCb mgsCb_;
};
