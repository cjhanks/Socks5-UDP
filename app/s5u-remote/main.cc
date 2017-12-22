#include <iostream>
#include <memory>
#include <thread>

#include <boost/fiber/all.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "reactor.hh"
#include "client-manager.hh"
#include "socket-tcp.hh"


DEFINE_string(
    local_host,
    "127.0.0.1",
    "");

DEFINE_int32(
    local_port,
    5000,
    "");


using namespace s5;

namespace {
void
GoRun()
{
  std::unique_ptr<Reactor> reactor(
      new Reactor(
        [&](AbstractSocket* client) {
          return new RemoteToLocal(reactor.get(), client);
        }));

  reactor->AddListener(new TcpListener(FLAGS_local_host, FLAGS_local_port));
  reactor->EventLoop();
}
} // ns


int
main(int argc, char* argv[])
{
  google::InstallFailureSignalHandler();
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  boost::fibers::fiber run_loop(GoRun);
  run_loop.join();

  LOG(INFO) << "Done";
  return 0;
}
