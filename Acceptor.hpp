#pragma once

#include "EventLoop.hpp"

class Acceptor : noncopyable {
public:
  Acceptor(EventLoop *loop, uint16_t port, const AcceptFinishCb &cb)
      : loop_(loop), port_(port), acceptfinishCb_(cb) {
    //����
    int listenfd = createAndListen(port_);

    //ע�ᡱ���������¼�����EventLoop
    // listenEvent_.reset(new Event(epfd_, listenfd));
    // listenEvent_ = new Event(epfd_, listenfd);
    listenEvent_.reset(new Event(loop_, listenfd));
    listenEvent_->setReadCallback(std::bind(&Acceptor::waketoListen, this));
    listenEvent_->add(EPOLLIN);
  }

private:
  //���������׽���
  int createAndListen(uint16_t port) {
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd = ::socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listenfd, F_SETFL, O_NONBLOCK); //����Ϊ��������

    bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 100);

    return listenfd;
  }

  //�����ӵ���¼�����ʱ���ص�
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
