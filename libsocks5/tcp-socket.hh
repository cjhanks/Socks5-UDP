#ifndef SOCKS5_TCP_SOCKET_H_
#define SOCKS5_TCP_SOCKET_H_

#include "socket.hh"


namespace s5 {
class Reactor;

class TcpRecvScoper : public AbstractScoper {
 public:
  TcpRecvScoper(int fd, Reactor* reactor);
  virtual ~TcpRecvScoper();

 private:
  int fd;
};

class TcpSendScoper : public AbstractScoper {
 public:
  TcpSendScoper(int fd, Reactor* reactor);
  virtual ~TcpSendScoper();

 private:
  int fd;
};

class TcpSocket : public AbstractSocket {
 public:
  TcpSocket(int fd, Reactor* reactor);
  virtual ~TcpSocket();

 protected:
  virtual ssize_t
  ImplRecv(std::uint8_t* data, std::size_t length);

  virtual ssize_t
  ImplSend(const std::uint8_t* data, std::size_t length);

  virtual AbstractScoper*
  NewRecvScoper();

  virtual AbstractScoper*
  NewSendScoper();

 private:
  int fd;
};
} // ns s5

#endif // SOCKS5_TCP_SOCKET_H_
