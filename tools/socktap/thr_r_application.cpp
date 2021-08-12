#include "thr_r_application.hpp"
#include <chrono>
#include <functional>
#include <iostream>

// This is a very simple application that sends BTP-B messages with maxmimal allowed payload and message counter.

using namespace vanetza;

extern vanetza::ByteBuffer g_pakholder;

Throughpout_Receiver::Throughpout_Receiver(boost::asio::io_service& io, std::chrono::milliseconds interval) :
    timer_(io), interval_(interval)
{
    schedule_timer();
}

Throughpout_Receiver::PortType Throughpout_Receiver::port()
{
    return host_cast<uint16_t>(43);
}

void Throughpout_Receiver::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    //extract counter from packet and call counter processing
    //data starts at byte 79
    counter = (g_pakholder[79]<<8)|(g_pakholder[80]);    
    counter_processor();
}

void Throughpout_Receiver::schedule_timer()
{
    timer_.expires_from_now(interval_);
    timer_.async_wait(std::bind(&Throughpout_Receiver::on_timer, this, std::placeholders::_1));
}

void Throughpout_Receiver::counter_processor()
{
    if(counter - counter_old > 1)
    {
        lost_counter++;
    }
    else
    {
        m_received_messages++;
    }

    counter_old = counter;
}

void Throughpout_Receiver::on_timer(const boost::system::error_code& ec)
{
    if (ec != boost::asio::error::operation_aborted) {
        
        //increment pass counter/seconds counter
        pass_counter++;

        //throughput calculation: msg_counter * msg_size (payload + 2 B. counter + 146 B. overhead) / (bytes in mbit)
        thrp = ((float)m_received_messages*(g_pakholder.size()))/125000;
        std::cout << "Pass:        " << pass_counter << std::endl;
        std::cout << "Packet size: " << g_pakholder.size() << " B" << std::endl;
        std::cout << "Received:    " << m_received_messages << " messages/second, ";
        std::cout << "lost: " << lost_counter << " messages" << std::endl;
        std::cout << "Throughput:  " << std::setprecision(3) << thrp << " MBit/s" << std::endl;
        
        std::cout << std::endl;
        m_received_messages = 0;
        lost_counter = 0;
        
        schedule_timer();
    }
}
