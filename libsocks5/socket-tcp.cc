#include "socket-tcp.hh"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <unistd.h>

#include "reactor.hh"


namespace s5 {
void
TcpSocket::SetNonBlocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  PCHECK(flags >= 0);
  flags = (flags | O_NONBLOCK);
  PCHECK(0 == fcntl(fd, F_SETFL, flags));
}

TcpSocket::TcpSocket(int fd)
  : fd(fd)
{
  CHECK_GE(fd, 0);
  SetNonBlocking(fd);
}

TcpSocket::~TcpSocket()
{
  DLOG(INFO) << "~TcpSocket()";
  if (fd >= 0)
    PCHECK(0 == ::close(fd));
}

ssize_t
TcpSocket::ImplRecv(std::uint8_t* data, std::size_t length)
{ return ::recv(fd, data, length, 0); }

ssize_t
TcpSocket::ImplSend(const std::uint8_t* data, std::size_t length)
{ return ::send(fd, data, length, 0); }

void
TcpSocket::Register(Reactor* reactor)
{ reactor->Register(this); }

void
TcpSocket::Unregister(Reactor* reactor)
{ reactor->Unregister(this); }


void
TcpSocket::ImplSetDeadTriggered(Reactor* reactor)
{
  reactor->Unregister(this);
}

// -------------------------------------------------------------------------- //

TcpListener::TcpListener(std::string ip, int port)
  : fd(-1)
{
  struct sockaddr_in server;
  server.sin_port        = htons(port);
  server.sin_addr.s_addr = ::inet_addr(ip.c_str());
  server.sin_family      = AF_INET;

  fd = ::socket(AF_INET, SOCK_STREAM, 0);
  PCHECK(fd > 0);

  static int True = 1;
  PCHECK(0 == ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &True, sizeof(True)));
  PCHECK(0 == ::bind(fd, (struct sockaddr*)&server, sizeof(server)));
  PCHECK(0 == ::listen(fd, SOMAXCONN));

  TcpSocket::SetNonBlocking(fd);
}

TcpListener::~TcpListener()
{
  DLOG(INFO) << "~TcpListener()";
  if (fd >= 0)
    PCHECK(0 == ::close(fd));
}

AbstractSocket*
TcpListener::Accept()
{
  struct sockaddr in_addr;
  socklen_t in_len = sizeof(in_addr);

  int client_fd = accept(fd, &in_addr, &in_len);
  if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return nullptr;
    else
      LOG(FATAL)
        << "Failed to accept: " << strerror(errno);
  }

  LOG(INFO) << client_fd;

  return new TcpSocket(client_fd);
}

void
TcpListener::Register(Reactor* reactor)
{
  reactor->AddListener(this);
}


} // ns s5
