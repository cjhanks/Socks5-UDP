#include "socket-abstract.hh"
#include "reactor.hh"


namespace s5 {
AbstractSocket::AbstractSocket()
  : dead(false)
  , recv_triggered(false)
  , send_triggered(false)
{}

std::size_t
AbstractSocket::Recv(std::uint8_t* data, std::size_t length, bool require_all)
{
  std::unique_lock<boost::fibers::mutex> guard(recv_mutex);
  std::size_t bytes_read = 0;
  while (bytes_read != length) {
    recv_condition.wait(guard, [&]() { return dead || recv_triggered; });
    if (dead)
      return 0;
    ssize_t rc = ImplRecv(data + bytes_read, length - bytes_read);
    if (rc < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        recv_triggered = false;
        if (!require_all)
          break;
      } else {
        LOG(FATAL)
            << "Failed to read data: " << rc << " (" << bytes_read
                                             << ":"  << length << ") "
                                             << strerror(errno);
      }
    } else {
      bytes_read += rc;
    }
  }

  return bytes_read;
}

void
AbstractSocket::SetRecvTriggered()
{
  //DLOG(INFO) << "SetRecvTriggered()";
  //CHECK(!recv_triggered);
  recv_triggered = true;
  recv_condition.notify_one();
}

std::size_t
AbstractSocket::Send(const std::uint8_t* data, std::size_t length, bool require_all)
{
  std::unique_lock<boost::fibers::mutex> guard(send_mutex);
  std::size_t bytes_sent = 0;
  while (bytes_sent != length) {
    send_condition.wait(guard, [&]() { return dead || send_triggered; });
    ssize_t rc = ImplSend(data + bytes_sent, length - bytes_sent);
    if (dead)
      return 0;
    if (rc < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        send_triggered = false;
        if (!require_all)
          break;
      } else {
        LOG(FATAL)
            << "Failed to sent data: " << rc;
      }
    } else {
      bytes_sent += rc;
    }
  }

  return bytes_sent;
}

void
AbstractSocket::SetSendTriggered()
{
  //DLOG(INFO) << "SetSendTriggered()";
  //CHECK(!send_triggered);
  send_triggered = true;
  send_condition.notify_one();
}

void
AbstractSocket::SetDeadTriggered(Reactor* reactor)
{
  dead = true;
  SetSendTriggered();
  SetRecvTriggered();

  ImplSetDeadTriggered(reactor);
}
} // ns s5
