#ifndef SOCKS5_REACTOR_H_
#define SOCKS5_REACTOR_H_

#include <functional>
#include <string>

#include <boost/fiber/all.hpp>

#include "socket.hh"


namespace s5 {
class AbstractManager;

class Reactor {
 public:
  using Generator = std::function<AbstractManager*(AbstractSocket*)>;

  Reactor(std::string ip, int port, Generator generator);

  /// Runs forever
  void
  Run();

  /// {
  /// TCP epoll registering and unregistering.
  void
  RegisterRecvTCP(boost::fibers::barrier* mutex, int fd);

  void
  UnRegisterRecvTCP(int fd);

  void
  RegisterSendTCP(boost::fibers::barrier* mutex, int fd);

  void
  UnRegisterSendTCP(int fd);
  /// }

 private:
  int epoll_id;
  int listener;
  Generator generator;

  void
  gConfigureAcceptSocketTCP(std::string ip, int port);

  void
  ConfigureEpoll();

  void
  HandleAcceptsTCP();
};

} // ns s5

#endif // SOCKS5_REACTOR_H_
