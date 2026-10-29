// HTTP request & response for BTI support.
//
// Copyright (C) 2026 Daniel Herrera Cabrera <dani_goo@hotmail.com>

#ifndef _BTI_HTTP_HTTP
#define _BTI_HTTP_HTTP

#include <string>
#include <unordered_map>

struct Request {

	std::string query_string;
	std::string method;
	std::string content_type;
	std::string content_length;
	std::string script_name;
	std::string uri;
	std::string document_uri;
	std::string document_root;
	std::string server_protocol;
	std::string scheme;
	std::string https;
	std::string gateway_interface;
	std::string server_software;
	std::string remote_addr;
	std::string remote_port;
	std::string server_addr;
	std::string server_port;
	std::string server_name;
	std::string server_version;
	std::string bti_manufacturer;
	std::string bti_product;
	std::string bti_version;
	std::string bti_component;
	std::unordered_map<std::string, std::string> params;
	std::unordered_map<std::string, std::string> headers;
	std::string body;

};

struct Response {
	int status = 200;
	std::string content_type = "application/json";
	std::string body;
};


#endif /* _BTI_HTTP_HTTP */