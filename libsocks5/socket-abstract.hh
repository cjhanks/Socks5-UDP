#ifndef ABSTRACT_SOCKET_HH_
#define ABSTRACT_SOCKET_HH_

#include <cstdint>
#include <stdexcept>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>


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

  /// {
  template <typename Struct>
  void
  Recv(Struct* s)
  { CHECK_EQ(Recv((std::uint8_t*)s, sizeof(Struct)), sizeof(Struct)); }

  std::size_t
  Recv(std::uint8_t* data, std::size_t length, bool require_all=true);

  void
  SetRecvTriggered();
  /// }

  /// {
  template <typename Struct>
  void
  Send(const Struct* s)
  { CHECK_EQ(Send((const std::uint8_t*)s, sizeof(Struct)), sizeof(Struct)); }

  std::size_t
  Send(const std::uint8_t* data, std::size_t length, bool require_all=true);

  void
  SetSendTriggered();
  /// }

  void
  SetDeadTriggered(Reactor* reactor);

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
