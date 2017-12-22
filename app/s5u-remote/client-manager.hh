#ifndef MANAGER_H_
#define MANAGER_H_

#include "handler-abstract.hh"
#include "socket-abstract.hh"

#include <memory>
#include <stdexcept>

#include "wire.pb.h"


namespace s5 {
class Reactor;

class GeneralFailure : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class RemoteToLocal : public AbstractHandler {
 public:
  RemoteToLocal(Reactor* reactor, AbstractSocket* client);
  virtual ~RemoteToLocal();

  virtual void
  Negotiate();

 private:
  void
  ImplNegotiate();

  void
  SendResponse(wire::ResStatus status);

  void
  HandleDNS(const wire::Req& dns);
};
} // ns s5

#endif // MANAGER_H_
