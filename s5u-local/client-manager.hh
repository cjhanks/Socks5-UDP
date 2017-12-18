#ifndef MANAGER_H_
#define MANAGER_H_

#include "handler-abstract.hh"
#include "socket-abstract.hh"

#include <memory>


namespace s5 {
class Reactor;

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
};
} // ns s5

#endif // MANAGER_H_
