#include "../../include/rabbitmq/amqp_publisher.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
using namespace AmqpClient;

namespace BSMCollectorAMQPClient {

    AMQP_Publisher::AMQP_Publisher(Channel::ptr_t channel, const string &exchange_name, const string &exchange_type, const string &queue_name) : m_channel(channel), m_exchangeName(exchange_name), m_exchangeType(exchange_type),  m_queueName(queue_name) {
        if (m_exchangeName == "") {
            m_exchangeName = "amq.direct";
            boost::uuids::random_generator uuid_gen;
            boost::uuids::uuid guid(uuid_gen());
            m_exchangeName += boost::lexical_cast<string>(guid);
        }

        // This is throwing error 403! Idk why
        // boost::string_ref pm(m_exchangeName);
        // m_channel->CheckExchangeExists(pm);

        m_channel->DeclareExchange(m_exchangeName, m_exchangeType, false, false, false);
        m_channel->DeclareQueue(m_queueName, false, true, false, false);
        m_channel->BindQueue(m_queueName, m_exchangeName, "");
    }

    AMQP_Publisher::~AMQP_Publisher() {}

    void AMQP_Publisher::Publish(const string &message, const string &uuid) {

        BasicMessage::ptr_t outgoing_message = BasicMessage::Create();

        Table headersTable;
        headersTable.insert(std::pair<string, string>("UUID", uuid));

        outgoing_message->HeaderTable(headersTable);
        outgoing_message->Body(message);

        Publish(outgoing_message);
    }

    void AMQP_Publisher::Publish(const BasicMessage::ptr_t message) {
        m_channel->BasicPublish(m_exchangeName, "", message);
    }

}