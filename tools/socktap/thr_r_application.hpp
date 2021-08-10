#ifndef THROUGHPOUT_RECEIVER_HPP_EUIC2VFR
#define THROUGHPOUT_RECEIVER_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

class Throughpout_Receiver : public Application, private Application::PromiscuousHook
{
public:
    Throughpout_Receiver(boost::asio::io_service&, std::chrono::milliseconds interval);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;
    Application::PromiscuousHook* promiscuous_hook() override;

private:

    void schedule_timer();
    void counter_processor();
    void on_timer(const boost::system::error_code& ec);
    void tap_packet(const DataIndication&, const vanetza::UpPacket&) override;

    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
    uint16_t counter_new = 0;
    uint16_t counter_old = 0;
    unsigned lost_counter;
    unsigned m_received_messages;
};

#endif /* THROUGHPOUT_RECEIVER_HPP_EUIC2VFR */
