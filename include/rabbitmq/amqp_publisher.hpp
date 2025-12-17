#ifndef _AMQP_PUBLISHER_H
#define _AMQP_PUBLISHER_H

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/Util.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

using namespace std;
using namespace AmqpClient;


namespace BSMCollectorAMQPClient {

    class AMQP_Publisher : boost::noncopyable {

        public:

            explicit AMQP_Publisher(Channel::ptr_t channel, const string &exchange_name, const string &exchange_type, const string &queue_name);

            typedef boost::shared_ptr<AMQP_Publisher> ptr_t;

            static ptr_t Create(Channel::ptr_t channel, const string &exchange_name = "bsm.collector_exchange_", const string &exchange_type = "direct", const string &queue_name = "bsm.collector_queue_") {
                return boost::make_shared<AMQP_Publisher>(channel, exchange_name, exchange_type, queue_name);
            }

            string getPublisherName() const {
                return m_exchangeName;
            }

            void Publish(const string &message, const string &uuid);
            void Publish(BasicMessage::ptr_t message);

        public:
            virtual ~AMQP_Publisher();

        private:
            Channel::ptr_t m_channel;
            string m_exchangeName;
            string m_exchangeType;
            string m_queueName;

    };

} // namespace BSMCollectorAMQPClient

#endif // _AMQP_PUBLISHER_H