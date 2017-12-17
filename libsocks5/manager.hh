#ifndef MANAGER_H_
#define MANAGER_H_

#include "tcp-socket.hh"

namespace s5 {
class Manager {
 public:
  static void
  Launch(TcpSocket* socket);

  explicit Manager(TcpSocket* socket);
  virtual ~Manager();

 private:
  TcpSocket* client;
};
} // ns s5

#endif // MANAGER_H_
