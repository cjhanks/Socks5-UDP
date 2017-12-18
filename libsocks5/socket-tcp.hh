#ifndef SOCKET_TCP_HH_
#define SOCKET_TCP_HH_

#include "socket-abstract.hh"


namespace s5 {
class Reactor;

/// @class TcpSocket
class TcpSocket : public AbstractSocket {
 public:
  static void
  SetNonBlocking(int fd);

  explicit TcpSocket(int fd);
  virtual ~TcpSocket();

  virtual void
  Register(Reactor* reactor);

  virtual void
  Unregister(Reactor* reactor);

 protected:
  virtual ssize_t
  ImplRecv(std::uint8_t* data, std::size_t length);

  virtual ssize_t
  ImplSend(const std::uint8_t* data, std::size_t length);

  virtual void
  ImplSetDeadTriggered(Reactor* reactor);

 private:
  friend class Reactor;
  int fd;
};

// -------------------------------------------------------------------------- //

/// @class TcpListener
class TcpListener : public AbstractListener {
 public:
  TcpListener(std::string ip, int port);
  virtual ~TcpListener();

  virtual AbstractSocket*
  Accept();

  virtual void
  Register(Reactor* reactor);

 private:
  friend class Reactor;
  int fd;
};
} // ns s5

#endif // SOCKET_TCP_HH_
