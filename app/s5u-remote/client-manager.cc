#include "client-manager.hh"

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <vector>

#include <glog/logging.h>

#include "reactor.hh"
#include "socks5.hh"
#include "socket-tcp.hh"


namespace s5 {
using namespace message;

RemoteToLocal::RemoteToLocal(Reactor* reactor, AbstractSocket* client)
  : AbstractHandler(reactor)
{
  SetClient(client);
}

RemoteToLocal::~RemoteToLocal()
{}

void
RemoteToLocal::Negotiate()
{
  try {
    ImplNegotiate();
    SendResponse(wire::SUCCESS);
  } catch (const SpecificSocketException& err) {
    SendResponse(err.Status());
    LOG(INFO)
        << "SpecificSocketException(...)";
    return;
  } catch (const Exception& err) {
    LOG(INFO)
        << "Exception received: " << err.what();
    SendResponse(wire::GENERAL_FAILURE);
    return;
  }

  DLOG(INFO) << "RemoteToLocal::SpawnProxy()";
  SpawnProxy();
}

void
RemoteToLocal::ImplNegotiate()
{
  std::uint32_t length;
  Client()->Recv(&length);

  std::string buffer;
  buffer.resize(length);
  Client()->Recv(buffer);

  wire::Req req;
  if (!req.ParseFromString(buffer))
    throw ProtocolException("Failed to parse container");

  switch (req.request_case()) {
  case wire::Req::kDns:
      HandleDNS(req);
      break;

    case wire::Req::kIpv4:
    case wire::Req::kIpv6:
    default:
      throw ProtocolException("Not implemented");
  }
}

void
RemoteToLocal::HandleDNS(const wire::Req& req)
{
  DLOG(INFO) << "HandleDNS(...)";
  auto dns = req.dns();

  // Do a SYNCHRONOUS DNS lookup (FIXME)
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  struct sockaddr_in* ipv4 = nullptr;

  struct addrinfo* res;
  int rc = getaddrinfo(dns.cname().c_str(), NULL, &hints, &res);
  if (0 != rc)
    throw SocketException("Failed to get address");

  for (auto p = res; p != nullptr; p = p->ai_next) {
    if (p->ai_family != AF_INET)
      continue;

    ipv4 = (struct sockaddr_in *)p->ai_addr;
    ipv4->sin_port = req.port();
    break;
  }

  LOG(INFO) << dns.cname() << ":" << htons(req.port());

  if (ipv4 == nullptr)
    throw SocketException("Failed to find DNS record");

  // -
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    throw SocketException("Failed to create new socket");

  if (connect(fd, (struct sockaddr*) ipv4, sizeof(sockaddr_in)) < 0)
    throw SpecificSocketException(wire::CONNECTOIN_REFUSED);

  SetRemote(new TcpSocket(fd));
}

void
RemoteToLocal::SendResponse(wire::ResStatus status)
{
  LOG(INFO) << "SendResponse(" << status << ")";
  wire::Res res;
  res.set_status(status);

  std::string buffer;
  CHECK(res.SerializeToString(&buffer))
      << "Failed to serialize";

  std::uint32_t length = buffer.size();

  Client()->Send(&length);
  Client()->Send(buffer);
}
} // ns
