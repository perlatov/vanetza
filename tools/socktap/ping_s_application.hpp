#ifndef PING_SENDER_APPLICATION_HPP_EUIC2VFR
#define PING_SENDER_APPLICATION_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>

class Ping_Sender : public Application
{
public:
    Ping_Sender(boost::asio::io_service&, std::chrono::milliseconds interval);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:
    
    boost::posix_time::ptime  start;
    boost::posix_time::ptime  finish;
    boost::posix_time::time_duration duration;

    uint8_t counter = 0;

    void schedule_timer();
    void calculate();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
};

#endif /* PING_SENDER_APPLICATION_HPP_EUIC2VFR */
