#ifndef MANAGER_H_
#define MANAGER_H_

#include "handler-abstract.hh"
#include "socket-abstract.hh"

#include <memory>
#include <stdexcept>


namespace s5 {
class Reactor;

class GeneralFailure : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class ClientManager : public AbstractHandler {
 public:
  ClientManager(Reactor* reactor);
  virtual ~ClientManager();

  virtual void
  Negotiate(AbstractSocket* socket);

 private:
  AbstractSocket* client;
  AbstractSocket* remote;

  void
  NegotiateClientMethod();

  void
  HandleIPV4();

  void
  HandleIPV6();

  void
  HandleDNS();

  void
  SpawnProxy();

  void
  SendFailureResponse();
};
} // ns s5

#endif // MANAGER_H_
