#include "handler-abstract.hh"

#include <memory>
#include <glog/logging.h>


namespace s5 {
void
AbstractHandler::NegotiateLoop(AbstractHandler* handler_, AbstractSocket* client)
{
  std::unique_ptr<AbstractHandler>(handler_)->Negotiate(client);
  LOG(INFO) << "EONegotiation";
}

AbstractHandler::AbstractHandler(Reactor* reactor)
  : reactor(reactor)
{}
} // ns s5
