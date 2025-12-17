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

#include "../../include/http_server/http_server_api.hpp"
#include "../../include/rabbitmq/amqp_publisher.hpp"

#include "../../include/http_server/http_server_utils.hpp"

using namespace std;

namespace fs = std::filesystem;

namespace novasoft_bti_base {

    char * http_request_query_string;
    char * http_request_method;
    char * http_request_content_type;
    char * http_request_content_length;
    char * http_request_script_name;
    char * http_request_uri;
    char * http_request_document_uri;
    char * http_request_document_root;
    char * http_request_server_protocol;
    char * http_request_scheme;
    char * http_request_https;
    char * http_request_gateway_interface;
    char * http_request_server_software;
    char * http_request_remote_addr;
    char * http_request_remote_port;
    char * http_request_server_addr;
    char * http_request_server_port;
    char * http_request_server_name;
    char * http_request_server_version;
    char * http_request_bsm_manufacturer;
    char * http_request_bsm_product;
    char * http_request_bsm_version;
    char * http_request_bsm_component;

    long sequence = 0;

    using namespace BSMCollectorAMQPClient;
    using namespace BSMCollectorUtils;

    HttpApi::HTTP_VERBS HttpApi::method_requested(char * method) {
        if (strcmp(method, "GET") == 0) {
            return HttpApi::HTTP_VERBS::GET;
        }
        else if (strcmp(method, "HEAD") == 0) {
            return HttpApi::HTTP_VERBS::HEAD;
        }
        else if (strcmp(method, "POST") == 0) {
            return HttpApi::HTTP_VERBS::POST;
        }
        else if (strcmp(method, "PUT") == 0) {
            return HttpApi::HTTP_VERBS::PUT;
        }
        else if (strcmp(method, "PATCH") == 0) {
            return HttpApi::HTTP_VERBS::PATCH;
        }
        else if (strcmp(method, "DELETE") == 0) {
            return HttpApi::HTTP_VERBS::DELETE;
        }
        else if (strcmp(method, "OPTIONS") == 0) {
            return HttpApi::HTTP_VERBS::OPTIONS;
        }
        else {
            return HttpApi::HTTP_VERBS::NOT_ALLOWED;
        }
    }

    string HttpApi::get_body_request() {
        long int body_length = atoi(http_request_content_length);
        string body{};
        char c;
        int i = 0;
        while(cin.get(c)) {
            body += c;
        }
        return body;
    }

    // GET method
    void HttpApi::GET_routes() {
        code_404();
    }

    // POST method
    void HttpApi::POST_routes() {

        ini::IniFile configIni;

        if(!fs::exists("config/bsm.ini")) {
            cout 	<< _BSM_HTTP_200
                    << "<h1>No existe!</h1>";
        }
        else {

            configIni.setMultiLineValues(true);
            configIni.load("config/bsm.ini");

            // AMQP Connection
            string bsm_amqp_channel_host = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_channel_host"].as<string>());
            int bsm_amqp_channel_port = configIni["amqp"]["bsm_amqp_channel_port"].as<int>();
            string bsm_amqp_channel_user = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_channel_user"].as<string>());
            string bsm_amqp_channel_pass = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_channel_pass"].as<string>());
            string bsm_amqp_channel_vhost = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_channel_vhost"].as<string>());

            // AMQP Exchange and Queue
            string bsm_amqp_exchange_name = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_exchange_name"].as<string>());
            string bsm_amqp_exchange_type = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_exchange_type"].as<string>());
            string bsm_amqp_queue_name = BSM_Utils::sanitize_params(configIni["amqp"]["bsm_amqp_queue_name"].as<string>());

            // HTTP
            string bsm_http_route = BSM_Utils::sanitize_params(configIni["http"]["bsm_http_route"].as<string>());

            // Events
            string bsm_mandatory_attributes_file = BSM_Utils::sanitize_params(configIni["events"]["bsm_mandatory_attributes_file"].as<string>());

            std::time_t received_timestamp = std::time(0);

            // /BSM/v1.0/collector
            if(strcmp((char *)bsm_http_route.c_str(), http_request_uri) == 0) {
                if(atoi(http_request_content_length) == 0) {
                    cout 	<< _BSM_HTTP_406
                            << "Content is empty!";
                }
                else {

                    if(strcmp((char *)"application/json", http_request_content_type) != 0) {
                        cout 	<< _BSM_HTTP_406
                                << "Content type not fit!";
                    }
                    else {

                        Channel::ptr_t channel = Channel::Create(
                            bsm_amqp_channel_host,
                            bsm_amqp_channel_port,
                            bsm_amqp_channel_user,
                            bsm_amqp_channel_pass,
                            bsm_amqp_channel_vhost
                        );

                        boost::uuids::random_generator uuid_gen;
                        boost::uuids::uuid guid(uuid_gen());
                        string uuid = boost::lexical_cast<std::string>(guid);

                        sequence = sequence + 1;

                        std::time_t now = std::time(0);
                        string time_format = (string)std::asctime(std::localtime(&now));
                        time_format = time_format.substr(0, time_format.size() - 1);

                        // Open config attributes and read content
                        ifstream mandatory_attributes_file_stream(bsm_mandatory_attributes_file);
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

                            reader.parse(HttpApi::get_body_request(), event);

                            collector["http"] = (string)http_request_server_software;
                            collector["http_version"] = (string)http_request_server_version;
                            collector["server_addr"] = (string)http_request_server_addr;
                            collector["server_port"] = (string)http_request_server_port;
                            collector["server_name"] = (string)http_request_server_name;
                            collector["remote_addr"] = (string)http_request_remote_addr;
                            collector["remote_port"] = (string)http_request_remote_port;
                            collector["current_timestamp"] = time_format;
                            collector["current_timestamp_epoch"] = to_string(now);

                            event_metadata["recived_timestamp"] = to_string(received_timestamp);
                            event_metadata["recived_sequence"] = to_string(sequence);

                            root["manufacturer"] = "Novasoft";
                            root["product"] = "BSM Community";
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
                            bsm_amqp_exchange_name,
                            bsm_amqp_exchange_type,
                            bsm_amqp_queue_name
                        );

                        pub->Publish(body, uuid);

                        cout 	<< "Content-type: text/html\r\n\r\n"
                                << uuid;
                    }
                }
            }

        }

    }

    // PUT method
    void HttpApi::PUT_routes() {
        code_405();
    }

    // PATCH method
    void HttpApi::PATCH_routes() {
        code_405();
    }

    // DELETE method
    void HttpApi::DELETE_routes() {
        code_405();
    }

    // HEAD method
    void HttpApi::HEAD_routes() {
        code_405();
    }

    // OPTIONS method
    void HttpApi::OPTIONS_routes() {
        code_405();
    }

    // Code 404 Not Found
    void HttpApi::code_404() {
        string template_404 = "html/404.html";
        if(BSM_Utils::file_exists(template_404)) {
            ifstream html_error_404_file_stream(template_404);
            string buffer;
            string html_template;
            while (getline(html_error_404_file_stream, buffer)) {
                html_template = html_template + buffer;
            }
            html_error_404_file_stream.close();

            cout 	<< _BSM_HTTP_404
                    << html_template;
        }
        else {
            cout 	<< _BSM_HTTP_404;
        }
    }

    // Code 405 Method Not Allowed
    void HttpApi::code_405() {
        if(BSM_Utils::file_exists("html/405.html")) {
            ifstream html_error_405_file_stream("html/405.html");
            string buffer;
            string html_template;
            while (getline(html_error_405_file_stream, buffer)) {
                html_template = html_template + buffer;
            }
            html_error_405_file_stream.close();

            cout 	<< _BSM_HTTP_405
                    << html_template;
        }
        else {
            cout 	<< _BSM_HTTP_405;
        }
    }

    // Router
    void HttpApi::router(FCGX_Request &request) {

        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

        http_request_query_string	    =	FCGX_GetParam("QUERY_STRING", request.envp);
        http_request_method     		=	FCGX_GetParam("REQUEST_METHOD", request.envp);
        http_request_content_type	    =	FCGX_GetParam("CONTENT_TYPE", request.envp);
        http_request_content_length		=	FCGX_GetParam("CONTENT_LENGTH", request.envp);
        http_request_script_name		=	FCGX_GetParam("SCRIPT_NAME", request.envp);
        http_request_uri    			=	FCGX_GetParam("REQUEST_URI", request.envp);
        http_request_document_uri	   	=	FCGX_GetParam("DOCUMENT_URI", request.envp);
        http_request_document_root 	   	=	FCGX_GetParam("DOCUMENT_ROOT", request.envp);
        http_request_server_protocol	=	FCGX_GetParam("SERVER_PROTOCOL", request.envp);
        http_request_scheme     		=	FCGX_GetParam("REQUEST_SCHEME", request.envp);
        http_request_https			   	=	FCGX_GetParam("HTTPS", request.envp);
        http_request_gateway_interface  =	FCGX_GetParam("GATEWAY_INTERFACE", request.envp);
        http_request_server_software	=	FCGX_GetParam("SERVER_SOFTWARE", request.envp);
        http_request_remote_addr		=	FCGX_GetParam("REMOTE_ADDR", request.envp);
        http_request_remote_port		=	FCGX_GetParam("REMOTE_PORT", request.envp);
        http_request_server_addr		=	FCGX_GetParam("SERVER_ADDR", request.envp);
        http_request_server_port		=	FCGX_GetParam("SERVER_PORT", request.envp);
        http_request_server_name		=	FCGX_GetParam("SERVER_NAME", request.envp);
        http_request_server_version     =   FCGX_GetParam("SERVER_VERSION", request.envp);
        http_request_bsm_manufacturer   =   FCGX_GetParam("BSM_MANUFACTURER", request.envp);
        http_request_bsm_product        =   FCGX_GetParam("BSM_PRODUCT", request.envp);
        http_request_bsm_version        =   FCGX_GetParam("BSM_VERSION", request.envp);
        http_request_bsm_component      =   FCGX_GetParam("BSM_COMPONENT", request.envp);

        try {

            auto method = HttpApi::method_requested(http_request_method);

            switch(method) {
                case HttpApi::HTTP_VERBS::GET :
                    GET_routes(); 
                break;
                case HttpApi::HTTP_VERBS::POST :
                    POST_routes();
                break;
                case HttpApi::HTTP_VERBS::PUT :
                    PUT_routes();
                break;
                case HttpApi::HTTP_VERBS::PATCH :
                    PATCH_routes();
                break;
                case HttpApi::HTTP_VERBS::DELETE :
                    DELETE_routes();
                break;
                case HttpApi::HTTP_VERBS::HEAD :
                    HEAD_routes();
                break;
                case HttpApi::HTTP_VERBS::OPTIONS :
                    OPTIONS_routes();
                break;
                default:
                    code_405();
                break;
            }


        }
        catch(const runtime_error& e) {
            cerr << e.what();
        }


    }

}