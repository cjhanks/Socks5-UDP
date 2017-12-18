#include "handler-abstract.hh"

#include <memory>


namespace s5 {
void
AbstractHandler::NegotiateLoop(AbstractHandler* handler_, AbstractSocket* client)
{ std::unique_ptr<AbstractHandler>(handler_)->Negotiate(client); }

AbstractHandler::AbstractHandler(Reactor* reactor)
  : reactor(reactor)
{}
} // ns s5
