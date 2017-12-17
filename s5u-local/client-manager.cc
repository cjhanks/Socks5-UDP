#include "client-manager.hh"

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <vector>

#include <glog/logging.h>

#include "structures.hh"


namespace s5 {
using namespace message;

ClientManager::ClientManager(AbstractSocket* socket)
  : client(socket)
{}

ClientManager::~ClientManager()
{
  DLOG(INFO) << "~ClientManager()";
}

void
ClientManager::Execute()
{
  NegotiateClientMethod();

  LOG(INFO) << "Starting request";
  ClientRequestHead request;
  client->Recv(&request);
  DLOG(INFO) << request;

  switch (request.address_type) {
    case AddressType::IPV4:
      HandleIPV4();
      break;
    case AddressType::IPV6:
      HandleIPV6();
      break;
    case AddressType::DOMAINNAME:
      HandleDNS();
      break;
    default:
      LOG(WARNING)
        << "Invalid request";
      return;
  }

  SpawnProxy();
}

void
ClientManager::NegotiateClientMethod()
{
  // Parse the client header.
  ClientHello hello;
  client->Recv(&hello);
  DLOG(INFO) << hello;

  if (0 == hello.number_of_methods)
    throw IOException("Invalid number of requests");

  // Get the current methods
  std::vector<Method> methods(hello.number_of_methods);
  client->Recv((std::uint8_t*) &methods[0], sizeof(methods[0]) * methods.size());

  bool noauth_found = false;
  for (const auto& method: methods) {
    if (method == Method::NO_AUTHENTICATION_REQUIRED)
      noauth_found = true;
    DLOG(INFO) << method;
  }

  // FIXME:  Always uses no auth
  ServerSelectMethod select;
  select.version = Version::SOCKS5;
  select.method  = Method::NO_AUTHENTICATION_REQUIRED;
  client->Send(&select);
}

void
ClientManager::HandleIPV4()
{
  LOG(FATAL) << "HandleIPV4";
}

void
ClientManager::HandleIPV6()
{
  LOG(FATAL) << "HandleIPV6";
}

void
ClientManager::HandleDNS()
{
  // Get the DNS request body
  ClientRequestBodyDomainName req;
  client->Recv(&req);
  //DLOG(INFO) << req;

  std::string url;
  url.resize(req.number_of_octets_which_follow);
  client->Recv((std::uint8_t*) &url[0], url.size());
  DLOG(INFO) << "URL: " << url;

  // Get the tail, not sure what it's for yet..
  ClientRequestTail tail;
  client->Recv(&tail);
  DLOG(INFO) << tail;


  // Do a SYNCHRONOUS DNS lookup (FIXME)
  std::uint32_t ip;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  struct sockaddr_in* ipv4;

  struct addrinfo* res;

  int rc = getaddrinfo(url.c_str(), NULL, &hints, &res);
  PCHECK(0 == rc)
      << "Rc = " << rc;

  for (auto p = res; p != nullptr; p = p->ai_next) {
    if (p->ai_family != AF_INET)
      continue;

    ipv4 = (struct sockaddr_in *)p->ai_addr;
    ipv4->sin_port = tail.port;
    LOG(INFO) << htons(tail.port);
    break;
  }

  // Send it back.
  DLOG(INFO) << "Responding";
  ServerResponse response;
  response.version      = Version::SOCKS5;
  response.reply        = ReplyMessage::SUCCESS;
  response.reserve      = 0;
  response.address_type = AddressType::IPV4;
  response.address      = 0;
  response.port         = 0;

  client->Send(&response);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  PCHECK(0 == connect(fd, (struct sockaddr*) ipv4, sizeof(*ipv4)));
  remote.reset(new TcpSocket(fd, client->GetReactor()));
}

void
ClientManager::SpawnProxy()
{
  CHECK(client);
  CHECK(remote);

  LOG(INFO) << "Launch fiber 0";
  boost::fibers::fiber(
  [](std::shared_ptr<AbstractSocket> client,
     std::shared_ptr<AbstractSocket> remote) {
    LOG(INFO) << "IN fiber 0";
    std::uint8_t buffer[1024];
    do {
      DLOG(INFO) << "F0";
      CHECK(client);
      CHECK(remote);
      try {
        std::size_t rc = remote->Recv(buffer, sizeof(buffer), false);
        LOG(INFO) << "R -> C: " << rc;
        client->Send(buffer, rc, true);
      } catch (...) {
        LOG(INFO) << "Exit 0";
        break;
      }
    } while (true);
  },
  client,
  remote).detach();

  LOG(INFO) << "Launch fiber 1";
  boost::fibers::fiber(
  [](std::shared_ptr<AbstractSocket> client,
     std::shared_ptr<AbstractSocket> remote) {
    LOG(INFO) << "IN fiber 1";
    std::uint8_t buffer[1024];
    do {
      DLOG(INFO) << "F1";
      CHECK(client);
      CHECK(remote);
      try {
        std::size_t rc = client->Recv(buffer, sizeof(buffer), false);
        LOG(INFO) << "C -> R: " << rc;
        remote->Send(buffer, rc, true);
      } catch (...) {
        LOG(INFO) << "Exit 1";
        break;
      }
    } while (true);
  },
  client,
  remote).detach();

}
} // ns
