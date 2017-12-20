#include "handler-abstract.hh"

#include <memory>
#include <glog/logging.h>

#include "reactor.hh"


namespace s5 {
void
AbstractHandler::NegotiateLoop(AbstractHandler* handler_)
{
  DLOG(INFO) << "NegotiateLoop(...)";
  std::unique_ptr<AbstractHandler>(handler_)->Negotiate();
}

AbstractHandler::AbstractHandler(Reactor* reactor)
  : reactor(reactor)
  , client(nullptr)
  , remote(nullptr)
{
  DCHECK_NOTNULL(reactor);
}

AbstractHandler::~AbstractHandler()
{
  delete client;
  delete remote;
}

void
AbstractHandler::SetClient(AbstractSocket* socket)
{
  CHECK(client == nullptr);
  client = socket;
  client->Register(reactor);
}

void
AbstractHandler::SetRemote(AbstractSocket* socket)
{
  CHECK(remote == nullptr);
  remote = socket;
  remote->Register(reactor);
}

void
AbstractHandler::SpawnProxy()
{
  boost::fibers::fiber RtoC(
  [](AbstractSocket* client, AbstractSocket* remote) {
    std::uint8_t buffer[4096];
    do {
      try {
        std::size_t rc = remote->Recv(buffer, sizeof(buffer), false);
        if (rc == 0)
          break;
        if (0 == client->Send(buffer, rc, true))
          break;
      } catch (...) {
        break;
      }
    } while (true);

    client->SetDeadTriggered();
    remote->SetDeadTriggered();
  },
  client,
  remote);

  boost::fibers::fiber CtoR(
  [](AbstractSocket* client, AbstractSocket* remote) {
    std::uint8_t buffer[4096];
    do {
      try {
        std::size_t rc = client->Recv(buffer, sizeof(buffer), false);
        if (rc == 0)
          break;
        if (0 == remote->Send(buffer, rc, true))
          break;
      } catch (...) {
        break;
      }
    } while (true);

    client->SetDeadTriggered();
    remote->SetDeadTriggered();
  },
  client,
  remote);

  RtoC.join();
  CtoR.join();
}
} // ns s5
