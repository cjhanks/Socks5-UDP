#ifndef SOCKS5_STRUCTURES_H_
#define SOCKS5_STRUCTURES_H_

#include <cstdint>


namespace s5 { namespace message {
// -------------------------------------------------------------------------- //
// Enumerations
// -------------------------------------------------------------------------- //
enum class Version : std::uint8_t {
  SOCKS4 = 0x04,
  SOCKS5 = 0x05,
};

enum class Method : std::uint8_t {
  NO_AUTHENTICATION_REQUIRED = 0x00,
  GSSAPI                     = 0x01,
  USER_PASS                  = 0x02,
  // 0x03 - 0x7F (IANA ASSIGNED)
  // 0x80 - 0xFE (Reserved for private use)
  NO_ACCEPTABLE_METHODS      = 0xFF,
};

enum class Command : std::uint8_t {
  CONNECT       = 0x01,
  BIND          = 0x02,
  UDP_ASSOCIATE = 0x03,
};

enum class AddressType : std::uint8_t {
  IPV4       = 0x01,
  DOMAINNAME = 0x03,
  IPV6       = 0x04,
};

enum class ReplyMessage : std::uint8_t {
  SUCCESS                = 0x00,
  GENERAL_FAILURE        = 0x01,
  CONNECTION_NOT_ALLOWED = 0x02,
  NETWORK_UNREACHABLE    = 0x03,
  HOST_UNREACHABLE       = 0x04,
  CONNECTOIN_REFUSED     = 0x05,
  TTL_EXPIRED            = 0x06,
  COMMAND_NOT_SUPPORTED  = 0x07,
  ADDRESS_NOT_SUPPORTED  = 0x08,
  // 0x09 - 0xFF unassigned
};

// -------------------------------------------------------------------------- //
// Structures
// -------------------------------------------------------------------------- //

/// The client connects to the server stating "Hello, I am using SOCKS5.  N
/// methods will follow".
struct ClientHello {
  Version       version;
  std::uint8_t  number_of_methods;
};


/// The server responds to the client specifying which method it prefers to use
/// for authentication.
struct ServerSelectMethod {
  Version version;
  Method  method;
};

/// {
/// The client would like to request a new connection from the server
struct ClientRequestHead {
  Version      version;
  Command      command;
  std::uint8_t reserve;
  AddressType  address_type;
};

/// If: address = IPV4
struct ClientRequestBodyIPV4 {
  std::uint32_t ipv4;
};

/// If: address = IPV6
struct ClientRequestBodyIPV6 {
  std::uint64_t ipv6_0;
  std::uint64_t ipv6_1;
};

/// If: address = DOMAINNAME
/// Additionally, there are `number_of_octets_which_follow` bytes which will
/// follow this structure.
struct ClientRequestBodyDomainName{
  std::uint8_t number_of_octets_which_follow;
};

struct ClientRequestTail {
  std::uint16_t port;
};
/// }

/// {
struct ServerResponseHead {
  Version      version;
  ReplyMessage reply;
  std::uint8_t reserve;
  AddressType  address_type;
};

struct ServerResponseBodyIPV4 : public ClientRequestBodyIPV4 {};
struct ServerResponseBodyIPV6 : public ClientRequestBodyIPV6 {};
struct ServerResponseBodyDomainName : public ClientRequestBodyDomainName {};
struct ServerResponseBodyIPV4 : public ClientRequestBodyIPV4 {};

struct ServerResponseTail {
  std::uint16_t port;
};
/// }

} // ns message
} // ns s5
