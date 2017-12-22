#ifndef ABSTRACT_SOCKET_HH_
#define ABSTRACT_SOCKET_HH_

#include <cstdint>
#include <stdexcept>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>

#include "exception.hh"


namespace s5 {
class Reactor;

class IOException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

/// @class AbstractSocket
class AbstractSocket {
 public:
  AbstractSocket();
  virtual ~AbstractSocket() = default;

  virtual void
  Connect(const std::string& host, int port) = 0;

  /// {
  template <typename Struct>
  void
  Recv(Struct* s)
  {
    if (Recv((std::uint8_t*)s, sizeof(Struct)), sizeof(Struct) != sizeof(Struct))
      throw SocketException("Invalid receive length");
  }

  void
  Recv(std::string& data)
  {
    if (Recv((std::uint8_t*)&data[0], data.size()) != data.size())
      throw SocketException("Invalid receive length");
  }

  std::size_t
  Recv(std::uint8_t* data, std::size_t length, bool require_all=true);

  void
  SetRecvTriggered();
  /// }

  /// {
  template <typename Struct>
  void
  Send(const Struct* s)
  {
    if (Send((const std::uint8_t*)s, sizeof(Struct)), sizeof(Struct) != sizeof(Struct))
      throw SocketException("Invalid send length");
  }

  void
  Send(const std::string& data)
  {
    if (Send((const std::uint8_t*)&data[0], data.size()) != data.size())
      throw SocketException("Invalid send length");
  }

  std::size_t
  Send(const std::uint8_t* data, std::size_t length, bool require_all=true);

  void
  SetSendTriggered();
  /// }

  void
  SetDeadTriggered();

  virtual void
  Register(Reactor* reactor) = 0;

  virtual void
  Unregister(Reactor* reactor) = 0;

 protected:
  /// {
  /// These methods must be overloaded by the implementing socket type.
  ///
  /// Their return type should either be the number of bytes read or EAGAIN.
  /// Any other response will trigger a disconnect.
  virtual ssize_t
  ImplRecv(std::uint8_t* data, std::size_t length) = 0;

  virtual ssize_t
  ImplSend(const std::uint8_t* data, std::size_t length) = 0;

  virtual void
  ImplSetDeadTriggered(Reactor* reactor) = 0;
  /// }

 private:
  bool dead;

  boost::fibers::mutex              recv_mutex;
  boost::fibers::condition_variable recv_condition;
  bool                              recv_triggered;

  boost::fibers::mutex              send_mutex;
  boost::fibers::condition_variable send_condition;
  bool                              send_triggered;
};

// -------------------------------------------------------------------------- //

///
class AbstractListener {
 public:
  AbstractListener() = default;

  virtual AbstractSocket*
  Accept() = 0;

  virtual void
  Register(Reactor* reactor) = 0;
};

} // ns s5

#endif // ABSTRACT_SOCKET_HH_
