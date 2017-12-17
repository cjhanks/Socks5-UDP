#include "socket.hh"

#include <cstring>
#include <mutex>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>


namespace s5 {
AbstractScoper::AbstractScoper(Reactor* reactor)
  : reactor(reactor)
  , mutex(1)
{}

void
AbstractScoper::Acquire()
{
  mutex.wait();
}

Reactor*
AbstractScoper::GetReactor()
{ return reactor; }

boost::fibers::barrier*
AbstractScoper::GetMutex()
{ return &mutex; }


AbstractSocket::AbstractSocket(Reactor* reactor)
  : reactor(reactor)
{}

Reactor*
AbstractSocket::GetReactor()
{
  return reactor;
}

std::size_t
AbstractSocket::Recv(std::uint8_t* data, std::size_t length)
{
  std::unique_ptr<AbstractScoper> scoper(NewRecvScoper());

  std::size_t bytes_read = 0;
  while (bytes_read != length) {
    scoper->Acquire();

    ssize_t rc = ImplRecv(data + bytes_read, length - bytes_read);
    if (rc < 0) {
      if (rc == EAGAIN || rc == EWOULDBLOCK) {
        continue;
      } else {
        LOG(WARNING)
            << "Failed to receive: " << rc << " (" << strerror(rc) << ")";
        throw IOException("Failed to recv");
      }
    } else {
      bytes_read += rc;
    }
  }

  return bytes_read;
}

std::size_t
AbstractSocket::Send(const std::uint8_t* data, std::size_t length)
{
  std::unique_ptr<AbstractScoper> scoper(NewSendScoper());

  std::size_t bytes_sent = 0;
  while (bytes_sent != length) {
    scoper->Acquire();

    ssize_t rc = ImplSend(data + bytes_sent, length - bytes_sent);
    if (rc < 0) {
      if (rc == EAGAIN || rc == EWOULDBLOCK) {
        continue;
      } else {
        LOG(WARNING)
            << "Failed to send : " << rc << " (" << strerror(rc) << ")";
        throw IOException("Failed to send");
      }
    } else {
      bytes_sent += rc;
    }
  }

  return bytes_sent;
}
} // ns s5
