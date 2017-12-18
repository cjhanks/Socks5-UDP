#include "reactor.hh"

#include <sys/epoll.h>

#include <cstdint>
#include <cstring>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>

#include "socket-tcp.hh"


namespace s5 {
struct Handle {
  enum Type {
    LISTENER,
    SOCKET,
  };

  Handle(AbstractListener* listener)
    : type(LISTENER)
  { u.listener = listener; }

  Handle(AbstractSocket* socket)
    : type(SOCKET)
  { u.socket = socket; }

  Type type;
  union {
    AbstractListener* listener;
    AbstractSocket* socket;
  } u;
};


Reactor::Reactor(Generator generator)
  : epoll_id(-1)
  , generator(generator)
{
  InitializeEpoll();
}

void
Reactor::EventLoop()
{
  DCHECK_GE(epoll_id, 0);

  static std::size_t constexpr MAXEVENTS = 128;
  std::vector<epoll_event> events(MAXEVENTS);

  do {
    int n = epoll_wait(epoll_id, &events[0], events.size(), 1);
    for (int i = 0; i < n; ++i) {
      epoll_event& ev = events[i];
      auto handle = (Handle*)ev.data.ptr;
      DCHECK_NOTNULL(handle);

      if (ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
        if (handle->type == Handle::SOCKET) {
          handle->u.socket->SetDeadTriggered(this);
          delete handle;
        } else {
          LOG(FATAL) << "Failed accepting socket";
        }
      } else {
        if (handle->type == Handle::SOCKET) {
          if (ev.events & EPOLLIN)
            handle->u.socket->SetRecvTriggered();
          if (ev.events & EPOLLOUT)
            handle->u.socket->SetSendTriggered();

        } else {
          HandleAccepts(handle->u.listener);
        }
      }
    }

    // allow fibers to execute.
    boost::this_fiber::yield();

  } while (true);
}

void
Reactor::AddListener(TcpListener* listener)
{
  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.ptr = new Handle(listener);

  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_ADD, listener->fd, &ev));
}

void
Reactor::InitializeEpoll()
{
  DCHECK_EQ(epoll_id, -1);
  epoll_id = epoll_create1(0);
  PCHECK(epoll_id >= 0);
}

void
Reactor::HandleAccepts(AbstractListener* listener)
{
  while (AbstractSocket* client = listener->Accept()) {
    client->Register(this);
    boost::fibers::fiber(
      &AbstractHandler::NegotiateLoop,
      generator(),
      client).detach();
  }
}

void
Reactor::Register(TcpSocket* socket)
{
  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP| EPOLLET;
  ev.data.ptr = new Handle(socket);

  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_ADD, socket->fd, &ev));
}

void
Reactor::Unregister(TcpSocket* socket)
{
  struct epoll_event ev;
  PCHECK(0 == epoll_ctl(epoll_id, EPOLL_CTL_DEL, socket->fd, &ev));
}
} // ns s5
