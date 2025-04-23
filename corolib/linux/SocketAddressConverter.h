#ifndef SOCKETADDRESSCONVERTER_H_    
#define SOCKETADDRESSCONVERTER_H_

#include <sys/socket.h>
#include <cstdint>
#include <cstring>

namespace corolib
{
    void convertToIpv4Address(const sockaddr& address, std::uint8_t* addressBytes, std::uint16_t& port);
    void convertFromIpv4Address(const std::uint8_t* addressBytes, const uint16_t port, sockaddr& address);
}

#endif