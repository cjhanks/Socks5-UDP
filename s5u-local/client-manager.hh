#ifndef MANAGER_H_
#define MANAGER_H_

#include "manager.hh"
#include "socket.hh"

#include <memory>


namespace s5 {
class ClientManager : public AbstractManager {
 public:
  explicit ClientManager(AbstractSocket* socket);
  virtual ~ClientManager();

  virtual void
  Execute();

 private:
  std::shared_ptr<AbstractSocket> client;
  std::shared_ptr<AbstractSocket> remote;

  void
  NegotiateClientMethod();

  void
  HandleIPV4();

  void
  HandleIPV6();

  void
  HandleDNS();

  void
  SpawnProxy();
};
} // ns s5

#endif // MANAGER_H_
