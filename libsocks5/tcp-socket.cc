#include "tcp-socket.hh"

#include <glog/logging.h>

#include "reactor.hh"


namespace s5 {
TcpRecvScoper::TcpRecvScoper(int fd, Reactor* reactor)
  : AbstractScoper(reactor)
  , fd(fd)
{ GetReactor()->RegisterRecvTCP(GetMutex(), fd); }

TcpRecvScoper::~TcpRecvScoper()
{
  GetReactor()->UnRegisterRecvTCP(fd);
}

TcpSendScoper::TcpSendScoper(int fd, Reactor* reactor)
  : AbstractScoper(reactor)
  , fd(fd)
{ GetReactor()->RegisterSendTCP(GetMutex(), fd); }

TcpSendScoper::~TcpSendScoper()
{ GetReactor()->UnRegisterSendTCP(fd); }

TcpSocket::TcpSocket(int fd, Reactor* reactor)
  : AbstractSocket(reactor)
  , fd(fd)
{
  CHECK_GE(fd, 0);
  CHECK_NOTNULL(reactor);
}

TcpSocket::~TcpSocket()
{
  DLOG(INFO) << "~TcpSocket()";
  PCHECK(0 == ::close(fd));
}

ssize_t
TcpSocket::ImplRecv(std::uint8_t* data, std::size_t length)
{ return ::read(fd, data, length); }

ssize_t
TcpSocket::ImplSend(const std::uint8_t* data, std::size_t length)
{ return ::write(fd, data, length); }

AbstractScoper*
TcpSocket::NewRecvScoper()
{
  return new TcpRecvScoper(fd, GetReactor());
}

AbstractScoper*
TcpSocket::NewSendScoper()
{
  return new TcpSendScoper(fd, GetReactor());
}

} // ns s5
