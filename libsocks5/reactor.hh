#ifndef REACTOR_HH_
#define REACTOR_HH_

#include <functional>

#include "socket-abstract.hh"
#include "handler-abstract.hh"


namespace s5 {
class TcpListener;
class TcpSocket;


class Reactor {
 public:
  using Generator = std::function<AbstractHandler*()>;
  Reactor(Generator generator);

  void
  EventLoop();

  /// {
  void
  AddListener(TcpListener* listener);

  void
  Register(TcpSocket* socket);

  void
  Unregister(TcpSocket* socket);
  /// }

 private:
  int epoll_id;
  Generator generator;

  void
  InitializeEpoll();

  void
  HandleAccepts(AbstractListener* listener);
};
} // ns s5

#endif // REACTOR_HH_
