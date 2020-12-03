#pragma once

#include "EventLoop.hpp"

class Acceptor : noncopyable {
public:
  Acceptor(EventLoop *loop, uint16_t port, const AcceptFinishCb &cb)
      : loop_(loop), port_(port), acceptfinishCb_(cb) {
    //监听
    int listenfd = createAndListen(port_);

    //注册”接受连接事件“进EventLoop
    // listenEvent_.reset(new Event(epfd_, listenfd));
    // listenEvent_ = new Event(epfd_, listenfd);
    listenEvent_.reset(new Event(loop_, listenfd));
    listenEvent_->setReadCallback(std::bind(&Acceptor::waketoListen, this));
    listenEvent_->add(EPOLLIN);
  }

private:
  //创建监听套接字
  int createAndListen(uint16_t port) {
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd = ::socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listenfd, F_SETFL, O_NONBLOCK); //设置为非阻塞。

    bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 100);

    return listenfd;
  }

  //“连接到达”事件唤醒时被回调
  void waketoListen() {
    int connfd = accept4(listenEvent_->getSockfd(), NULL, NULL,
                         SOCK_CLOEXEC | SOCK_NONBLOCK);

    acceptfinishCb_(connfd);
  }

private:
  EventLoop *loop_;
  std::unique_ptr<Event> listenEvent_;
  uint16_t port_;
  AcceptFinishCb acceptfinishCb_;
};
