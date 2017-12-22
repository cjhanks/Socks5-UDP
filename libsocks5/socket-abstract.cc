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
  DCHECK_GT(length, 0);
  std::unique_lock<boost::fibers::mutex> guard(recv_mutex);
  std::size_t bytes_read = 0;
  while (bytes_read != length) {
    recv_condition.wait(guard, [&]() { return dead || recv_triggered; });
    if (dead) {
      return 0;
    }

    ssize_t rc = ImplRecv(data + bytes_read, length - bytes_read);
    if (rc < 0) {
      if (rc == -EAGAIN || rc == -EWOULDBLOCK) {
        recv_triggered = false;
        if (!require_all && bytes_read > 0)
          break;
      } else {
        throw SocketException(strerror(-rc));
      }
    } else
    if (rc == 0) {
      break;
    } else {
      bytes_read += rc;
    }
  }

  return bytes_read;
}

void
AbstractSocket::SetRecvTriggered()
{
  recv_triggered = true;
  recv_condition.notify_one();
}

std::size_t
AbstractSocket::Send(const std::uint8_t* data, std::size_t length, bool require_all)
{
  DCHECK_GT(length, 0);
  std::unique_lock<boost::fibers::mutex> guard(send_mutex);
  std::size_t bytes_sent = 0;
  while (bytes_sent != length) {
    send_condition.wait(guard, [&]() { return dead || send_triggered; });
    if (dead) {
      return 0;
    }

    ssize_t rc = ImplSend(data + bytes_sent, length - bytes_sent);
    if (rc < 0) {
      if (rc == -EAGAIN || rc == -EWOULDBLOCK) {
        send_triggered = false;
        if (!require_all && bytes_sent > 0)
          break;
      } else {
        throw SocketException(strerror(-rc));
      }
    } else
    if (rc == 0) {
      break;
    } else {
      bytes_sent += rc;
    }
  }


  return bytes_sent;
}

void
AbstractSocket::SetSendTriggered()
{
  //CHECK(!send_triggered);
  send_triggered = true;
  send_condition.notify_one();
}

void
AbstractSocket::SetDeadTriggered()
{
  dead = true;
  SetSendTriggered();
  SetRecvTriggered();
}
} // ns s5
