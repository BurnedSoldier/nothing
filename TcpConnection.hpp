#pragma once

#include "Buffer.hpp"
#include "EventLoop.hpp"

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop *loop, int sockfd, const OnMessageCb &mgsCb)
      : loop_(loop), connEvent_(loop_, sockfd), onMessageCb_(mgsCb) {
    connEvent_.setReadCallback(std::bind(&TcpConnection::wakeupRead, this));
    connEvent_.add(EPOLLIN);
  }

  void send(std::string mgs) {
    writeBuffer_.push(mgs.c_str(), mgs.size());
    connEvent_.setWriteCallback(std::bind(&TcpConnection::wakeupWrite, this));
    connEvent_.modifly(EPOLLOUT);
  }

 private:
  void wakeupRead() {
    char buf[10000];
    size_t sz = read(connEvent_.getSockfd(), buf, 10000);
    if (sz == 0) {
		shutdown(connEvent_.getSockfd(), SHUT_WR);
		connEvent_.del(EPOLLIN);

    } else {
      readBuffer_.push(buf, sz);

      auto connptr = shared_from_this();
      onMessageCb_(connptr, readBuffer_);
    }
  }

  void wakeupWrite() {
    size_t len;
    char *buf = writeBuffer_.getMgs(&len);
    size_t n = write(connEvent_.getSockfd(), buf, len);
    writeBuffer_.releaseBytes(n);
    connEvent_.modifly(EPOLLIN);
  }

 private:
  EventLoop *loop_;
  Event connEvent_;
  OnMessageCb onMessageCb_;
  RingBuffer readBuffer_;
  RingBuffer writeBuffer_;
};
