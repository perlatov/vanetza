#include "ping_r_application.hpp"
#include <chrono>
#include <functional>
#include <iostream>

// This is a very simple application that sends BTP-B messages with counter.

using namespace vanetza;

extern vanetza::ByteBuffer g_pakholder;

Ping_Responder::Ping_Responder(boost::asio::io_service& io) :
    timer_(io)
{
    //nothing to do here
    std::cout << "\nAwaiting for probes ... " << std::endl;
}

Ping_Responder::PortType Ping_Responder::port()
{
    return host_cast<uint16_t>(44);
}

void Ping_Responder::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    on_probe();
}

void Ping_Responder::on_probe()
{
        probe_counter++;
        DownPacketPtr packet { new DownPacket() };
        packet->layer(OsiLayer::Application) = ByteBuffer { 0xAF, probe_counter, 0xAE };
        DataRequest request;
        request.transport_type = geonet::TransportType::SHB;
        request.communication_profile = geonet::CommunicationProfile::ITS_G5;
        request.its_aid = aid::CA;
        auto confirm = Application::request(request, std::move(packet));
        if (!confirm.accepted()) {
            throw std::runtime_error("Ping Responder data request failed");
        }  
        std::cout << "\nResponse #" << std::to_string(probe_counter) << " sent! \n";
        if(probe_counter == 10)
        exit(EXIT_SUCCESS);   
}
