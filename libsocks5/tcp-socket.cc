#include "tcp-socket.hh"

#include <glog/logging.h>

#include "reactor.hh"


namespace s5 {
void
SetNonBlocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  PCHECK(flags >= 0);

  flags = (flags | O_NONBLOCK);
  PCHECK(0 == fcntl(fd, F_SETFL, flags));
}


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
  , fd_recv(fd)
  , fd_send(dup(fd))
{
  CHECK_GE(fd, 0);
  CHECK_NOTNULL(reactor);

  SetNonBlocking(fd_recv);
  SetNonBlocking(fd_send);
}

TcpSocket::~TcpSocket()
{
  DLOG(INFO) << "~TcpSocket()";
  PCHECK(0 == ::close(fd_recv));
  PCHECK(0 == ::close(fd_send));
}

ssize_t
TcpSocket::ImplRecv(std::uint8_t* data, std::size_t length)
{ return ::read(fd_recv, data, length); }

ssize_t
TcpSocket::ImplSend(const std::uint8_t* data, std::size_t length)
{ return ::write(fd_send, data, length); }

AbstractScoper*
TcpSocket::NewRecvScoper()
{
  return new TcpRecvScoper(fd_recv, GetReactor());
}

AbstractScoper*
TcpSocket::NewSendScoper()
{
  return new TcpSendScoper(fd_send, GetReactor());
}

} // ns s5
