#ifndef SOCKS5_REACTOR_H_
#define SOCKS5_REACTOR_H_

#include <string>
#include <boost/fiber/all.hpp>


namespace s5 {
class Reactor {
 public:
  Reactor(std::string ip, int port);

  /// Runs forever
  void
  Run();

  /// {
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

  void
  ConfigureAcceptSocket(std::string ip, int port);

  void
  ConfigureEpoll();

  void
  HandleAccepts();
};

} // ns s5

#endif // SOCKS5_REACTOR_H_
