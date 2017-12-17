#include "manager.hh"

#include <glog/logging.h>


namespace s5 {
void
AbstractManager::Handler(AbstractManager* handler_)
{
  std::unique_ptr<AbstractManager> handler(handler_);
  try {
    handler->Execute();
  } catch (const IOException& err) {
    LOG(WARNING)
        << "Unhandled IOError: " << err.what();
  }
}
} // ns s5
