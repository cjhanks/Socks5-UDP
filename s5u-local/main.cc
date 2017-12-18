#include <iostream>
#include <memory>
#include <thread>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>

#include "reactor.hh"
#include "client-manager.hh"
#include "socket-tcp.hh"


using namespace s5;

namespace {
void
GoRun()
{
  std::unique_ptr<Reactor> reactor(
      new Reactor(
        [&]() { return new ClientManager(reactor.get()); }));

  reactor->AddListener(new TcpListener("127.0.0.1", 5000));
  reactor->EventLoop();
}
} // ns


int
main(int argc, char* argv[])
{
  google::InstallFailureSignalHandler();
  boost::fibers::fiber run_loop(GoRun);
  run_loop.join();

  LOG(INFO) << "Done";
  return 0;
}
