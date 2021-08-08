#include "thr_s_application.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

// This is a very simple application that sends BTP-B messages with maxmimal allowed payload and message counter.

using namespace vanetza;

Throughpout_Sender::Throughpout_Sender(boost::asio::io_service& io, std::chrono::milliseconds interval) :
    timer_(io), interval_(interval)
{
    schedule_timer();
}

Throughpout_Sender::PortType Throughpout_Sender::port()
{
    return host_cast<uint16_t>(42);
}

void Throughpout_Sender::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    std::cout << "Throughpout Server App received a packet" << std::endl;
}

void Throughpout_Sender::schedule_timer()
{
    timer_.expires_from_now(interval_);
    timer_.async_wait(std::bind(&Throughpout_Sender::on_timer, this, std::placeholders::_1));
}

void Throughpout_Sender::message_counter()
{
    //couint to 2000, then reset to zero. 
    //counter is packed in first two bytes of msg payload and separated with zero byte
    if (counter == 0x7D0)
    {
        counter = 0;
    }
    else
    {
        counter++;
    }

    counter_split[0] = counter >> 8;    //lsb
    counter_split[1] = counter;         //msb
}

void Throughpout_Sender::on_timer(const boost::system::error_code& ec)
{
    if (ec != boost::asio::error::operation_aborted) {

        message_counter();

        DownPacketPtr packet { new DownPacket() };
        packet->layer(OsiLayer::Application) = ByteBuffer {counter_split[0], counter_split[1], 0x0 /*payload*/};

        //DownPacket packet;
        //packet.layer(OsiLayer::Application) = ByteBuffer { 0xC0, 0xFF, 0xEE };

        DataRequest request;
        request.transport_type = geonet::TransportType::SHB;
        request.communication_profile = geonet::CommunicationProfile::ITS_G5;
        request.its_aid = aid::CA;
        auto confirm = Application::request(request, std::move(packet));
        if (!confirm.accepted()) {
            throw std::runtime_error("Throughpout Client App data request failed");
        }

        schedule_timer();
    }
}
