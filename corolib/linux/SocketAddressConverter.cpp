#include "SocketAddressConverter.h"
#include <netinet/in.h>

namespace corolib
{
    void convertToIpv4Address(const sockaddr& address, std::uint8_t* addressBytes, std::uint16_t& port)
    {
        if (address.sa_family == AF_INET)
        {
            const auto& ipv4Address = reinterpret_cast<const sockaddr_in&>(address);
            std::memcpy(addressBytes, &ipv4Address.sin_addr, 4);
            port = ntohs(ipv4Address.sin_port);
        }
    }

    
    void convertFromIpv4Address(const std::uint8_t* addressBytes, const uint16_t port, sockaddr& address)
    {
        auto& ipv4Address = reinterpret_cast<sockaddr_in&>(address);
        ipv4Address.sin_family = AF_INET;
        ipv4Address.sin_port = htons(port);
        std::memcpy(&ipv4Address.sin_addr, addressBytes, 4);
    }
}
