
#ifndef PING_RESPONDER_HPP_EUIC2VFR
#define PING_RESPONDER_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

class Ping_Responder : public Application
{
public:
    Ping_Responder(boost::asio::io_service&);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:
    void schedule_timer();
    void on_probe();

    uint8_t probe_counter = 0;

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
};

#endif /* PING_RESPONDER_HPP_EUIC2VFR */
