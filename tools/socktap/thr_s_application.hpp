#ifndef THROUGHPOUT_SENDER_HPP_EUIC2VFR
#define THROUGHPOUT_SENDER_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

class Throughpout_Sender : public Application
{
public:
    Throughpout_Sender(boost::asio::io_service&, std::chrono::milliseconds interval);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:

    unsigned int counter = 0;
    unsigned char counter_split[2];
    unsigned char payload[14] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE}; 

    void schedule_timer();
    void message_counter();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
};

#endif /* THROUGHPOUT_SENDER_HPP_EUIC2VFR */
