#include <iostream>
#include <memory>
#include <thread>

#include <boost/fiber/all.hpp>
#include <glog/logging.h>

#include "reactor.hh"

using namespace s5;

namespace {
void
GoRun()
{
  std::unique_ptr<Reactor> reactor(new Reactor("127.0.0.1", 5000));
  reactor->Run();
}
} // ns


int
main(int argc, char* argv[])
{
  std::cerr << "OK\n";
  boost::fibers::fiber run_loop(GoRun);
  run_loop.join();

  LOG(INFO) << "Done";
}
