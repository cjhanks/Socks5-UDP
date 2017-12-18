#include "socks5.hh"


namespace s5 { namespace message {
std::ostream&
operator<<(std::ostream& ostr, const Version& version)
{
  switch (version) {
  case Version::SOCKS4:
    ostr << "SOCKS4";
    break;

  case Version::SOCKS5:
    ostr << "SOCKS5";
    break;
  }

  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const Command& command)
{
  switch (command) {
    case Command::CONNECT:
        ostr << "CONNECT";
        break;
    case Command::BIND:
        ostr << "BIND";
        break;
    case Command::UDP_ASSOCIATE:
        ostr << "UDP_ASSOCIATE";
        break;
  }

  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const AddressType& address)
{
  switch (address) {
  case AddressType::IPV4:
      ostr << "IPV4";
      break;
  case AddressType::DOMAINNAME:
      ostr << "DOMAINNAME";
      break;
  case AddressType::IPV6:
      ostr << "IPV6";
      break;
  }

  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const ReplyMessage& reply)
{
  switch (reply) {
    case ReplyMessage::SUCCESS:                ostr << "SUCCESS"; break;
    case ReplyMessage::GENERAL_FAILURE:        ostr << "GENERAL_FAILURE"; break;
    case ReplyMessage::CONNECTION_NOT_ALLOWED: ostr << "CONNECTION_NOT_ALLOWED"; break;
    case ReplyMessage::NETWORK_UNREACHABLE:    ostr << "NETWORK_UNREACHABLE"; break;
    case ReplyMessage::HOST_UNREACHABLE:       ostr << "HOST_UNREACHABLE"; break;
    case ReplyMessage::CONNECTOIN_REFUSED:     ostr << "CONNECTOIN_REFUSED"; break;
    case ReplyMessage::TTL_EXPIRED:            ostr << "TTL_EXPIRED"; break;
    case ReplyMessage::COMMAND_NOT_SUPPORTED:  ostr << "COMMAND_NOT_SUPPORTED"; break;
    case ReplyMessage::ADDRESS_NOT_SUPPORTED:  ostr << "ADDRESS_NOT_SUPPORTED"; break;
  }

  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const Method& method)
{
  switch (method) {
    case Method::NO_AUTHENTICATION_REQUIRED: ostr << "NO_AUTHENTICATION_REQUIRED"; break;
    case Method::GSSAPI                    : ostr << "GSSAPI"; break;
    case Method::USER_PASS                 : ostr << "USER_PASS"; break;
    case Method::NO_ACCEPTABLE_METHODS     : ostr << "NO_ACCEPTABLE_METHODS"; break;
  }

  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const ClientHello& hello)
{
  ostr << "ClientHello<" << int(hello.version) << ", "
                         << int(hello.number_of_methods) << ">";
  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const ClientRequestHead& req)
{
  ostr << "ClientRequestHead<" << int(req.version) << ", "
                               << req.command << ", "
                               << req.address_type << ">";
  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const ClientRequestTail& tail)
{
  ostr << "ClientRequestTail<" << tail.port << ">";
  return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const ServerResponse& resp)
{
  ostr << "ServerResponseBody<" << resp.version << ", "
                                << resp.reply << ", "
                                << resp.address_type << ", "
                                << resp.address << ", "
                                << resp.port << ">";
  return ostr;
}
} // ns message
} // ns s5
