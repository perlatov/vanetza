#include "thr_r_application.hpp"
#include <chrono>
#include <functional>
#include <iostream>

// This is a very simple application that sends BTP-B messages with maxmimal allowed payload and message counter.

using namespace vanetza;

Throughpout_Receiver::Throughpout_Receiver(boost::asio::io_service& io, std::chrono::milliseconds interval) :
    timer_(io), interval_(interval)
{
    schedule_timer();
}

Throughpout_Receiver::PortType Throughpout_Receiver::port()
{
    return host_cast<uint16_t>(43);
}

Application::PromiscuousHook* Throughpout_Receiver::promiscuous_hook()
{
    return this;
}

void Throughpout_Receiver::tap_packet(const DataIndication& indication, const UpPacket& packet)
{
    ++m_received_messages;
}

void Throughpout_Receiver::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    vanetza::byte_view_range bytes = create_byte_view(*packet, OsiLayer::Application);    
    std::cout << "payload lenght: " << bytes.size() << std::endl;
}

void Throughpout_Receiver::schedule_timer()
{
    timer_.expires_from_now(interval_);
    timer_.async_wait(std::bind(&Throughpout_Receiver::on_timer, this, std::placeholders::_1));
}

void Throughpout_Receiver::counter_processor()
{
    if(counter_new - counter_old > 1)
    {
        lost_counter++;
    }
    else
    {
        m_received_messages++;
    }

    counter_old = counter_new;

}

void Throughpout_Receiver::on_timer(const boost::system::error_code& ec)
{
    if (ec != boost::asio::error::operation_aborted) {
        
        std::cout << "Received " << m_received_messages << " messages/second" << std::endl;
        std::cout << "Lost " << lost_counter << " messages/second" << std::endl;
        m_received_messages = 0;
        lost_counter = 0;
        
        schedule_timer();
    }
}
