#include "ping_s_application.hpp"
#include <functional>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <ctime>
#include <boost/thread/thread_time.hpp>

// This is a very simple application that sends BTP-B messages with ony byte counter.

using namespace vanetza;

extern vanetza::ByteBuffer g_pakholder;

Ping_Sender::Ping_Sender(boost::asio::io_service& io, std::chrono::milliseconds interval) :
    timer_(io), interval_(interval)
{
    std::cout << "\nSending message with 3 B payload and awaiting for it to return, in 10 probes \n" << std::endl;
    schedule_timer();
}

Ping_Sender::PortType Ping_Sender::port()
{
    return host_cast<uint16_t>(44);
}

void Ping_Sender::calculate()
{
    finish = boost::posix_time::microsec_clock::local_time();
    duration = finish - start;
    
    std::cout << "Response #" << std::to_string(g_pakholder[79]) << " received" << std::endl;
    std::cout << "Round-trip time: " << duration.total_milliseconds() << "ms. " << duration.total_microseconds() << " μs.\n" << std::endl;

    if(g_pakholder[79] == 10)
    {
        exit(EXIT_SUCCESS);  
    }
}

void Ping_Sender::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    calculate();
}

void Ping_Sender::schedule_timer()
{
    timer_.expires_from_now(interval_);
    timer_.async_wait(std::bind(&Ping_Sender::on_timer, this, std::placeholders::_1));
}

void Ping_Sender::on_timer(const boost::system::error_code& ec)
{
    if (ec != boost::asio::error::operation_aborted) {
        DownPacketPtr packet { new DownPacket() };
        packet->layer(OsiLayer::Application) = ByteBuffer { 0xAE, counter, 0xAE };
        DataRequest request;
        request.transport_type = geonet::TransportType::SHB;
        request.communication_profile = geonet::CommunicationProfile::ITS_G5;
        request.its_aid = aid::CA;
        auto confirm = Application::request(request, std::move(packet));
        
        if (!confirm.accepted()) {
            throw std::runtime_error("Ping Sender application data request failed");
        }

        counter++;
        start = boost::posix_time::microsec_clock::local_time();
        std::cout << "Probe    #" << std::to_string(counter) << " sent" << std::endl;

        schedule_timer();

        if (counter==11)
        {
             std::cout << "\nsomething gone wrong\n" << std::endl;
            exit(EXIT_SUCCESS);  
        }

    }
}
