#ifndef THROUGHPOUT_RECEIVER_HPP_EUIC2VFR
#define THROUGHPOUT_RECEIVER_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

class Throughpout_Receiver : public Application
{
public:
    Throughpout_Receiver(boost::asio::io_service&, std::chrono::milliseconds interval);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:

    void schedule_timer();
    void counter_processor();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
    uint16_t counter;
    uint16_t counter_old;
    float    thrp{0.0f};
    unsigned lost_counter;
    unsigned m_received_messages;
    unsigned pass_counter;
};

#endif /* THROUGHPOUT_RECEIVER_HPP_EUIC2VFR */
