#include "ping_s_application.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

// This is a very simple application that sends BTP-B messages with maxmimal allowed payload and message counter.

using namespace vanetza;

Ping_Sender::Ping_Sender(boost::asio::io_service& io, std::chrono::milliseconds interval) :
    timer_(io), interval_(interval)
{
    schedule_timer();
}

Ping_Sender::PortType Ping_Sender::port()
{
    return host_cast<uint16_t>(42);
}

void Ping_Sender::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    //theoretically rx packet payload should be processed here - but i have no idea how to access and handle it
}

void Ping_Sender::schedule_timer()
{
    timer_.expires_from_now(interval_);
    timer_.async_wait(std::bind(&Ping_Sender::on_timer, this, std::placeholders::_1));
}

void Ping_Sender::get_time()
{
    time = std::time(0);

    time_split[0] = time >> 60;    
    time_split[1] = time >> 52;        
    time_split[2] = time >> 48;    
    time_split[3] = time >> 40;    
    time_split[4] = time >> 32;    
    time_split[5] = time >> 24;    
    time_split[6] = time >> 16;    
    time_split[7] = time >> 8; 
    time_split[8] = time; 
}

void Ping_Sender::on_timer(const boost::system::error_code& ec)
{
    if (ec != boost::asio::error::operation_aborted) {

        get_time();

        DownPacketPtr packet { new DownPacket() };
        packet->layer(OsiLayer::Application) = ByteBuffer 
        {time_split[0], time_split[1], time_split[2], time_split[3], time_split[4],
         time_split[5], time_split[6], time_split[7], time_split[8], 0xCA, 0xAC /*payload*/};

        DataRequest request;
        request.transport_type = geonet::TransportType::SHB;
        request.communication_profile = geonet::CommunicationProfile::ITS_G5;
        request.its_aid = aid::CA;
        auto confirm = Application::request(request, std::move(packet));
        if (!confirm.accepted()) {
            throw std::runtime_error("Throughpout sender data request failed");
        }

        schedule_timer();
    }
}
