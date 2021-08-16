#include "ethernet_device.hpp"
#include <algorithm>
#include <cstring>
#include <system_error>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <iostream>


static void initialize(ifreq& request, const char* interface_name)
{
    std::memset(&request, 0, sizeof(ifreq));
    std::strncpy(request.ifr_name, interface_name, IF_NAMESIZE);
    request.ifr_name[IF_NAMESIZE - 1] = '\0';
}

EthernetDevice::EthernetDevice(const char* devname, unsigned int opt1, unsigned int opt2, unsigned int opt3) :
    local_socket_(::socket(opt3, opt2, 0)),
    interface_name_(devname)
{
    const int IPTOS_EXPEDITED_FORWARDING = 0xb8;
    const int serviceType = IPTOS_EXPEDITED_FORWARDING;      
    setsockopt(local_socket_, IPPROTO_IP, IP_TOS, (const char*)&serviceType, sizeof(serviceType));
    setsockopt(local_socket_, SOL_SOCKET , SO_PRIORITY , (int*)&opt1, sizeof(opt1));
    getsockopt(local_socket_, SOL_SOCKET, SO_TYPE, (int*) &optval, &optlen);
    std::cout << "selected SO_TYPE: " << optval << std::endl;
    getsockopt(local_socket_, SOL_SOCKET, SO_PRIORITY, (int*) &optval, &optlen);
    std::cout << "selected SO_PRIORITY: " << optval << std::endl;
    getsockopt(local_socket_, SOL_SOCKET, SO_DOMAIN, (int*) &optval, &optlen);
    std::cout << "selected SO_DOMAIN: " << optval << std::endl;

    if (!local_socket_) {
        throw std::system_error(errno, std::system_category());
    }
}

EthernetDevice::~EthernetDevice()
{
    if (local_socket_ >= 0)
        ::close(local_socket_);
}

EthernetDevice::protocol::endpoint EthernetDevice::endpoint(int family) const
{
    sockaddr_ll socket_address = {0};
    socket_address.sll_family = family;
    socket_address.sll_protocol = htons(ETH_P_ALL);
    socket_address.sll_ifindex = index();
    //socket_address.
    return protocol::endpoint(&socket_address, sizeof(sockaddr_ll));
}

int EthernetDevice::index() const
{
    ifreq data;
    initialize(data, interface_name_.c_str());
    ::ioctl(local_socket_, SIOCGIFINDEX, &data);
    return data.ifr_ifindex;
}

vanetza::MacAddress EthernetDevice::address() const
{
    ifreq data;
    initialize(data, interface_name_.c_str());
    ::ioctl(local_socket_, SIOCGIFHWADDR, &data);

    vanetza::MacAddress addr;
    std::copy_n(data.ifr_hwaddr.sa_data, addr.octets.size(), addr.octets.data());
    return addr;
}
