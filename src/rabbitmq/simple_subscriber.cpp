#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "../../include/rabbitmq/simple_subscriber.hpp"

namespace AmqpClient
{

    SimpleSubscriber::SimpleSubscriber(Channel::ptr_t channel, const std::string &publisher_name) : m_channel(channel) {
        m_consumerQueue = "bti.collector_queue_";
        m_channel->DeclareQueue(m_consumerQueue, false, true, false, false);
        m_channel->BindQueue(m_consumerQueue, publisher_name, "");
        m_channel->BasicConsume(m_consumerQueue, m_consumerQueue);
    }

    SimpleSubscriber::~SimpleSubscriber() {}

    std::string SimpleSubscriber::WaitForMessageString(int timeout) {
        BasicMessage::ptr_t incoming = WaitForMessage(timeout);
        return incoming->Body();
    }

    BasicMessage::ptr_t SimpleSubscriber::WaitForMessage(int timeout) {
        Envelope::ptr_t envelope;
        m_channel->BasicConsumeMessage(m_consumerQueue, envelope, timeout);
        return envelope->Message();
    }

} //namespace AmqpClient
