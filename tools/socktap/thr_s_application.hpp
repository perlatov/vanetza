#ifndef THROUGHPOUT_SENDER_HPP_EUIC2VFR
#define THROUGHPOUT_SENDER_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

//#define PAYLOAD 1365 //max allowed payload (+2 B counter), 1392 B for PC  

class Throughpout_Sender : public Application
{
public:
    Throughpout_Sender(boost::asio::io_service&, std::chrono::milliseconds interval, unsigned payload);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:
    uint16_t counter = 0;
    void schedule_timer();
    void message_counter();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
};

#endif /* THROUGHPOUT_SENDER_HPP_EUIC2VFR */
