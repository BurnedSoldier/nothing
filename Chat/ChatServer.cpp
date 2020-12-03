
#include "../TcpServer.hpp"

#include <iostream>
#include <set>

class ChatServer {
public:
  ChatServer(EventLoop *loop, uint16_t port) : server_(loop, port) {}

  void start() {
    server_.start(std::bind(&ChatServer::OnConnection, this, _1),
                  std::bind(&ChatServer::OnMessage, this, _1, _2));
  }

private:
  void OnConnection(TcpConnectionPtr &connptr) {
    userSet_.insert(connptr);
    std::cout << "accept : " << id++ << std::endl;
  }

  void OnMessage(TcpConnectionPtr &connptr, RingBuffer &buffer) {

  /*
   *	具有tcp拆包功能,必须搭配具有相同tcp消息格式客户端。
   */
    // while (buffer.readableBytes() > MaxHeadLen) {
    //   const char *sz = buffer.readableStart();
    //   uint32_t sz2 = atoi(sz);
    //   uint32_t sz3 = ntohl(sz2);
    //   if (buffer.readableBytes() >= MaxHeadLen + sz3) {
    // std::string mgs(buffer.readableStart(), sz3 + MaxHeadLen);
    // buffer.releaseBytes(sz3 + MaxHeadLen);
    // sendAll(mgs);
    //   } else {
    //     break;
    //   }
    // }

    /*
     *   不具有tcp拆包功能，收到多少字节数据都当成一个消息处理。
	 *   好处是可以使用telnet调试
	 *
	 *	 在这里因为网络库没有实现发起连接的功能，所以无法实现客户端。
	 *	 故这里就不拆包了。
     */
    size_t len;
    const char *ch = buffer.getMgs(&len);
    std::string mgs(ch, len);
    sendAll(mgs);
    buffer.releaseBytes(len);
  }

  void sendAll(std::string mgs) {
    for (auto i : userSet_) {
      i->send(mgs);
    }
  }

private:
  TcpServer server_;
  std::set<TcpConnectionPtr> userSet_;
  static constexpr uint32_t MaxHeadLen = 4;
  uint32_t id = 1;
};

int main() {
  EventLoop loop;
  ChatServer chat(&loop, 3001);
  chat.start();
  loop.loop();
}

