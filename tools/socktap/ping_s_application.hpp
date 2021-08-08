#ifndef PING_SENDER_HPP_EUIC2VFR
#define PING_SENDER_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

class Ping_Sender : public Application
{
public:
    Ping_Sender(boost::asio::io_service&, std::chrono::milliseconds interval);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:

    std::time_t time;
    std::time_t time_split[8];
    unsigned char payload[1500] = { 0xEE }; 

    void schedule_timer();
    void get_time();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
};

#endif /* PING_SENDER_HPP_EUIC2VFR */
