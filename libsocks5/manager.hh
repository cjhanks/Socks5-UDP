#ifndef ABSTRACT_MANAGER_H_
#define ABSTRACT_MANAGER_H_

#include "socket.hh"
#include "tcp-socket.hh"

namespace s5 {
class AbstractManager {
 public:
  static void
  Handler(AbstractManager* handler);
  virtual ~AbstractManager() = default;

  virtual void
  Execute() = 0;
};
} // ns s5

#endif // ABSTRACT_MANAGER_H_
