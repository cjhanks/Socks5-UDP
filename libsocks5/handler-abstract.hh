#ifndef HANDLER_ABSTRACT_HH_
#define HANDLER_ABSTRACT_HH_

#include "socket-abstract.hh"

namespace s5 {
class Reactor;

class AbstractHandler {
 public:
  static void
  NegotiateLoop(AbstractHandler* handler);

  AbstractHandler(Reactor* reactor);
  virtual ~AbstractHandler();

  virtual void
  Negotiate() = 0;;

 protected:
  Reactor*
  GetReactor()
  { return reactor; }

  AbstractSocket*
  Client()
  { return client; }

  AbstractSocket*
  Remote()
  { return remote; }

  void
  SpawnProxy();

  void
  SetClient(AbstractSocket* socket);

  void
  SetRemote(AbstractSocket* socket);

 private:
  Reactor* reactor;
  AbstractSocket* client;
  AbstractSocket* remote;
};
} // ns

#endif // HANDLER_ABSTRACT_HH_
