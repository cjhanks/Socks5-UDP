#include "reactor.hh"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <unistd.h>

#include <memory>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>

#include "manager.hh"


namespace s5 {
namespace {
// -------------------------------------------------------------------------- //
void
SetNonBlocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  PCHECK(flags >= 0);

  flags = (flags | O_NONBLOCK);
  PCHECK(0 == fcntl(fd, F_SETFL, flags));
}

} // ns


// -------------------------------------------------------------------------- //

Reactor::Reactor(std::string ip, int port, Reactor::Generator generator)
  : epoll_id(-1)
  , listener(-1)
  , generator(generator)
{
  gConfigureAcceptSocketTCP(ip, port);
  ConfigureEpoll();
}

void
Reactor::Run()
{
  static std::size_t constexpr MAXEVENTS = 128;
  std::vector<epoll_event> events(MAXEVENTS);
  do {
    int n = epoll_wait(epoll_id, &events[0], events.size(), -1);
    for (int i = 0; i < n; ++i) {
      epoll_event& ev = events[i];

      if (ev.events & EPOLLERR
       || ev.events & EPOLLHUP) {
        LOG(INFO)
          << "TODO: Handle HUP or ERR";
      } else
      if (ev.data.fd == listener) {
        HandleAcceptsTCP();
      } else {
        LOG(INFO) << "Set waiter";
        auto mutex = (boost::fibers::barrier*) ev.data.ptr;
        mutex->wait();
      }

      // allow fibers to execute.
      boost::this_fiber::yield();
    }
  } while (true);
}

void
Reactor::RegisterRecvTCP(boost::fibers::barrier* mutex, int fd)
{
  //DLOG(INFO) << "RegisterRecvTcp(..., " << fd << ")";
  struct epoll_event ev;
  ev.data.ptr = mutex;
  ev.events   = EPOLLIN;
  CHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_ADD, fd, &ev));
}

void
Reactor::UnRegisterRecvTCP(int fd)
{
  //DLOG(INFO) << "UnRegisterRecvTcp(..., " << fd << ")";
  struct epoll_event ev;
  ev.events = EPOLLIN;
  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_DEL, fd, &ev));
}

void
Reactor::RegisterSendTCP(boost::fibers::barrier* mutex, int fd)
{
  //DLOG(INFO) << "RegisterSendTcp(..., " << fd << ")";
  struct epoll_event ev;
  ev.data.ptr = mutex;
  ev.events   = EPOLLOUT;
  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_ADD, fd, &ev));
}

void
Reactor::UnRegisterSendTCP(int fd)
{
  //DLOG(INFO) << "UnRegisterSend(..., " << fd << ")";
  struct epoll_event ev;
  ev.events = EPOLLOUT;
  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_DEL, fd, &ev));
}

void
Reactor::gConfigureAcceptSocketTCP(std::string ip, int port)
{
  DCHECK_EQ(listener, -1);

  struct sockaddr_in server;
  server.sin_port        = htons(port);
  server.sin_addr.s_addr = ::inet_addr(ip.c_str());
  server.sin_family      = AF_INET;

  listener = ::socket(AF_INET, SOCK_STREAM, 0);
  PCHECK(listener > 0);

  static int True = 1;
  PCHECK(0 == ::setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &True, sizeof(True)));
  PCHECK(0 == ::bind(listener, (struct sockaddr*)&server, sizeof(server)));
  PCHECK(0 == ::listen(listener, SOMAXCONN));

  SetNonBlocking(listener);
}

void
Reactor::ConfigureEpoll()
{
  DCHECK_EQ(epoll_id, -1);
  DCHECK_GE(listener,  0);

  epoll_id = epoll_create1(0);
  PCHECK(epoll_id >= 0);

  struct epoll_event event;
  event.data.fd = listener;
  event.events  = EPOLLIN;

  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_ADD, listener, &event));
}

void
Reactor::HandleAcceptsTCP()
{
  do {
    struct sockaddr in_addr;
    socklen_t in_len = sizeof(in_addr);

    int fd = accept(listener, &in_addr, &in_len);
    if (fd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      else
        LOG(FATAL)
          << "Failed to accept: " << strerror(errno);
    } else {
      SetNonBlocking(fd);
      AbstractManager* manager = generator(new TcpSocket(fd, this));
      boost::fibers::fiber f(AbstractManager::Handler, manager);
      f.detach();
    }
  } while (true);
}
} // ns s5


