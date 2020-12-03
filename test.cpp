
#include "TcpServer.hpp"

#include <iostream>

uint32_t num = 1;
std::string head = "HTTP/1.0 OK 200\r\n\r\n<html> <html/>";

void onConnection(TcpConnectionPtr &connptr) {
  std::cout << "accept num : " << num++ << std::endl;
}

void onMessage(TcpConnectionPtr &connptr, RingBuffer &buffer) {
  size_t len;
  std::string mgs(buffer.getMgs(&len));
  buffer.releaseBytes(len);
  connptr->send(mgs);

}

int main() {
  EventLoop loop;
  TcpServer server(&loop, 3001);
  server.start(std::bind(&onConnection, _1), std::bind(&onMessage, _1, _2));
  loop.loop();
}

