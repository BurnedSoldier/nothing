#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std::placeholders;

class TcpConnection;
class RingBuffer;
class EventLoop;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using WakeToListenOrReadCb = std::function<void()>;
using WakeToWriteCb = std::function<void()>;
using AcceptFinishCb = std::function<void(int)>;
using OnMessageCb =
    std::function<void(TcpConnectionPtr &connptr, RingBuffer &buffer)>;
using OnConnection = std::function<void(TcpConnectionPtr &connptr)>;

class noncopyable {
 public:
  noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
};

class Event {
 public:
  Event(EventLoop *loop, int sockfd) : loop_(loop), sockfd_(sockfd) {}

  //返回管理的套接字
  int getSockfd() const { return sockfd_; }

  //设置唤醒事件
  void setRevent(uint32_t revent) { revent_ = revent; }

  //设置读(接受连接)回调
  void setReadCallback(const WakeToListenOrReadCb &cb) {
    waketoListenOrReadCb_ = cb;
  }
  //设置读(接受连接)回调
  void setWriteCallback(const WakeToWriteCb &cb) { wakeupWritecb_ = cb; }

  void add(uint32_t events);
  void modifly(uint32_t events);
  void del(uint32_t events);

  //调用回调 ： 读(接受连接） 或 写
  void handleCallback() {
    if (revent_ == EPOLLIN) {
      waketoListenOrReadCb_();  //接受连接或读取数据。
    } else if (revent_ == EPOLLOUT) {
      wakeupWritecb_();  //设置写事件。
    }
  }

  //返回注册的事件
  uint32_t getEvents() const { return event_; }

 private:
  EventLoop *loop_;
  int sockfd_;
  uint32_t event_;
  uint32_t revent_;
  WakeToListenOrReadCb waketoListenOrReadCb_;  //读（接受连接）回调指针
  WakeToWriteCb wakeupWritecb_;
};

class Epoll {
 public:
  Epoll() : epfd_(epoll_create(1024)) {}

  ~Epoll() { close(epfd_); }

  // epoll_wait
  std::vector<Event *> poll() {
    int nfds = epoll_wait(epfd_, eventNum, 500, -1);
    std::vector<Event *> evLst;
    for (int i = 0; i < nfds; ++i) {
      Event *ev = static_cast<Event *>(eventNum[i].data.ptr);
      ev->setRevent(eventNum[i].events);
      evLst.push_back(ev);
    }
    return evLst;
  }

  // epoll_ctl
  void modifly(Event *ev) {
    struct epoll_event epev;
    epev.data.ptr = ev;
    epev.events = ev->getEvents();
    epoll_ctl(epfd_, EPOLL_CTL_MOD, ev->getSockfd(), &epev);
  }

  void del(Event *ev) {
    struct epoll_event epev;
    epev.data.ptr = ev;
    epev.events = ev->getEvents();
    epoll_ctl(epfd_, EPOLL_CTL_DEL, ev->getSockfd(), &epev);
  }

  void add(Event *ev) {
    struct epoll_event epev;
    epev.data.ptr = ev;
    epev.events = ev->getEvents();
    epoll_ctl(epfd_, EPOLL_CTL_ADD, ev->getSockfd(), &epev);
  }

 private:
  int epfd_;
  struct epoll_event eventNum[500];
};

class EventLoop {
 public:
  void loop() {
    while (1) {
      std::vector<Event *> evlst_ = epoll_.poll();
      for (auto i : evlst_) {
        i->handleCallback();
      }
    }
  }

  void add(Event *ev) { epoll_.add(ev); }
  void modifly(Event *ev) { epoll_.modifly(ev); }
  void del(Event *ev) { epoll_.del(ev); }

 private:
  Epoll epoll_;
};

// inline void Event::canRead() {
//   event_ = EPOLLIN;
//   loop_->update(this);
// }
// inline void Event::canWrite() {
//   event_ = EPOLLOUT;
//   std::cout << "canWrite() " << std::endl;
//   loop_->update(this);
// }

inline void Event::add(uint32_t events) {
  event_ = events;
  loop_->add(this);
}
//设置写事件
inline void Event::modifly(uint32_t events) {
  event_ = events;
  loop_->modifly(this);
}

inline void Event::del(uint32_t events) {
  event_ = events;
  loop_->del(this);
}
