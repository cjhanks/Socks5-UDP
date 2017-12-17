#ifndef SOCKS5_SOCKET_H_
#define SOCKS5_SOCKET_H_

#include <cstdint>
#include <stdexcept>

#include <boost/fiber/all.hpp>


namespace s5 {
class Reactor;

class IOException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};


/// @class AbstractScoper
class AbstractScoper {
  public:
   AbstractScoper(Reactor* reactor);
   virtual ~AbstractScoper() = default;

   virtual void
   Acquire();

  protected:
   Reactor*
   GetReactor();

   boost::fibers::barrier*
   GetMutex();

  private:
   boost::fibers::barrier mutex;
   Reactor* reactor;
};


/// @class AbstractSocket
class AbstractSocket {
 public:
  AbstractSocket(Reactor* reactor);

  std::size_t
  Recv(std::uint8_t* data, std::size_t length);

  std::size_t
  Send(const std::uint8_t* data, std::size_t length);

 protected:
  Reactor*
  GetReactor();

  /// {
  /// These methods must be overloaded by the implementing socket type.
  ///
  /// Their return type should either be the number of bytes read OR EAGAIN.
  /// Any other response will trigger a disconnect.
  virtual ssize_t
  ImplRecv(std::uint8_t* data, std::size_t length) = 0;

  virtual ssize_t
  ImplSend(const std::uint8_t* data, std::size_t length) = 0;
  /// }

  ///
  virtual AbstractScoper*
  NewRecvScoper() = 0;

  virtual AbstractScoper*
  NewSendScoper() = 0;

 private:
  Reactor* reactor;
};
} // ns s5

#endif // SOCKS5_SOCKET_H_
