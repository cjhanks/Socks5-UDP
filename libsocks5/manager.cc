#include "manager.hh"

#include <glog/logging.h>


namespace s5 {
void
Manager::Launch(TcpSocket* socket)
{
  std::unique_ptr<TcpSocket> client(socket);

  while (true) {
    char c;
    try {
      client->Recv((std::uint8_t*)&c, sizeof(c));
      LOG(INFO) << c;
    } catch (const IOException& e) {
      break;
    }
  }
}

Manager::Manager(TcpSocket* socket)
  : client(socket)
{}

Manager::~Manager()
{
  delete client;
}
} // ns s5
