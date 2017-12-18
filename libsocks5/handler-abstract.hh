#ifndef HANDLER_ABSTRACT_HH_
#define HANDLER_ABSTRACT_HH_


namespace s5 {
class AbstractSocket;
class Reactor;

class AbstractHandler {
 public:
  static void
  NegotiateLoop(AbstractHandler* handler, AbstractSocket* socket);

  AbstractHandler(Reactor* reactor);

  virtual void
  Negotiate(AbstractSocket* client) = 0;;

 protected:
  Reactor*
  GetReactor()
  { return reactor; }

 private:
  Reactor* reactor;
};
} // ns

#endif // HANDLER_ABSTRACT_HH_
