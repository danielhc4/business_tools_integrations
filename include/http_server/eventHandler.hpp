// HTTP response codes for BTI support.
//
// Copyright (C) 2026 Daniel Herrera Cabrera <dani_goo@hotmail.com>
#ifndef _BTI_EVENTHANDLER
#define _BTI_EVENTHANDLER

#include "http.hpp"
#include <string_view>
#include <string.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <inicpp.h>
#include <filesystem>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>

#include <SimpleAmqpClient/SimpleAmqpClient.h>

#include "fcgio.h"

#include "../../include/rabbitmq/amqp_publisher.hpp"

#include "http_server_utils.hpp"

using namespace std;
using namespace BTICollectorAMQPClient;
using namespace BTICollectorUtils;

namespace fs = std::filesystem;

long sequence = 0;

class EventHandler
{

    public:
        static void publish(const Request& rq, Response& rs) {


            ini::IniFile configIni;

            if(!fs::exists("config/bti.ini")) {
                rs.status = 404;
                rs.body = R"({"error":"config/bti.ini not found"})";
            }
            else {

                configIni.setMultiLineValues(true);
                configIni.load("config/bti.ini");

                // AMQP Connection
                string bti_amqp_channel_host = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_channel_host"].as<string>());
                int bti_amqp_channel_port = configIni["amqp"]["bti_amqp_channel_port"].as<int>();
                string bti_amqp_channel_user = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_channel_user"].as<string>());
                string bti_amqp_channel_pass = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_channel_pass"].as<string>());
                string bti_amqp_channel_vhost = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_channel_vhost"].as<string>());

                // AMQP Exchange and Queue
                string bti_amqp_exchange_name = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_exchange_name"].as<string>());
                string bti_amqp_exchange_type = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_exchange_type"].as<string>());
                string bti_amqp_queue_name = BTI_Utils::sanitize_params(configIni["amqp"]["bti_amqp_queue_name"].as<string>());

                // HTTP
                string bti_http_route = BTI_Utils::sanitize_params(configIni["http"]["bti_http_route"].as<string>());

                // Events
                string bti_mandatory_attributes_file = BTI_Utils::sanitize_params(configIni["events"]["bti_mandatory_attributes_file"].as<string>());

                std::time_t received_timestamp = std::time(0);

                if(atoi(rq.content_length.c_str()) == 0) {
                    rs.status = 406;
                    rs.body = R"({"error":"Content is empty!"})";
                }
                else {

                    if(strcmp((char *)"application/json", rq.content_type.c_str()) != 0) {
                        rs.status = 406;
                        rs.body = R"({"error":"Content type not fit!"})";
                    }
                    else {

                        Channel::ptr_t channel = Channel::Create(
                            bti_amqp_channel_host,
                            bti_amqp_channel_port,
                            bti_amqp_channel_user,
                            bti_amqp_channel_pass,
                            bti_amqp_channel_vhost
                        );

                        boost::uuids::random_generator uuid_gen;
                        boost::uuids::uuid guid(uuid_gen());
                        string uuid = boost::lexical_cast<std::string>(guid);

                        sequence = sequence + 1;

                        std::time_t now = std::time(0);
                        string time_format = (string)std::asctime(std::localtime(&now));
                        time_format = time_format.substr(0, time_format.size() - 1);

                        // Open config attributes and read content
                        ifstream mandatory_attributes_file_stream(bti_mandatory_attributes_file);
                        string buffer;
                        string custom_attributes_string;
                        while (getline(mandatory_attributes_file_stream, buffer)) {
                            custom_attributes_string = custom_attributes_string + buffer;
                        }
                        mandatory_attributes_file_stream.close();

                        // Convert string to json
                        custom_attributes_string;
                        Json::Value custom_attributes_json;
                        Json::Reader custom_attributes_reader;
                        custom_attributes_reader.parse(custom_attributes_string, custom_attributes_json);

                        string body;

                        try {

                            Json::Value root;
                            Json::Value event;
                            Json::Value collector;
                            Json::Value event_metadata;
                            Json::Reader reader;

                            reader.parse(rq.body, event);

                            collector["http"] = rq.server_software;
                            collector["http_version"] = rq.server_version;
                            collector["server_addr"] = rq.server_addr;
                            collector["server_port"] = rq.server_port;
                            collector["server_name"] = rq.server_name;
                            collector["remote_addr"] = rq.remote_addr;
                            collector["remote_port"] = rq.remote_port;
                            collector["current_timestamp"] = time_format;
                            collector["current_timestamp_epoch"] = to_string(now);

                            event_metadata["recived_timestamp"] = to_string(received_timestamp);
                            event_metadata["recived_sequence"] = to_string(sequence);

                            root["manufacturer"] = "Novasoft";
                            root["product"] = "bti Community";
                            root["version"] = "1.0.0";
                            root["collector"] = collector;
                            root["event_metadata"] = event_metadata;
                            root["event"] = event;
                            root["events"] = custom_attributes_json;

                            Json::FastWriter fastWriter;
                            body = fastWriter.write(root);
                        }
                        catch(const std::exception& e) {

                            cerr << e.what() << '\n';
                            body = "{}";

                        }

                        boost::shared_ptr<AMQP_Publisher> pub = AMQP_Publisher::Create(
                            channel,
                            bti_amqp_exchange_name,
                            bti_amqp_exchange_type,
                            bti_amqp_queue_name
                        );

                        pub->Publish(body, uuid);

                        rs.status = 202;
                        rs.body = uuid;
                    }
                }

            }
        }
    
};

#endif /* _BTI_EVENTHANDLER */