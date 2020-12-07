
#include <iostream>

#include "TcpServer.hpp"

size_t id = 1;

void fuck(TcpConnectionPtr& connptr) {
  std::cout << "accept id :" << id++ << std::endl;
}

void you(TcpConnectionPtr& connptr, RingBuffer& buffer) {
  size_t len;
  char* str = buffer.getMgs(&len);
  std::string mgs(str, len);
  buffer.releaseBytes(len);
  connptr->send(mgs);
}

int main() {
  EventLoop loop;
  TcpServer server(&loop, 3000);
  server.start(std::bind(&fuck, _1), std::bind(&you, _1, _2));
  loop.loop();
}
