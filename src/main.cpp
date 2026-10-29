#include <iostream>
#include "fcgio.h"
#include <fcgiapp.h>
#include "http_server_router.hpp"
#include "eventHandler.hpp"

using namespace std;
using namespace novasoft_bti_base;

static std::string param(FCGX_Request& r, const char* name) {
    const char* v = FCGX_GetParam(name, r.envp);
    return v ? v : "";
}

static std::string read_body(FCGX_Request& r, size_t max_len = 1 << 20) {
    std::string len_s = param(r, "CONTENT_LENGTH");
    size_t len = len_s.empty() ? 0 : std::strtoul(len_s.c_str(), nullptr, 10);
    if (len == 0 || len > max_len) return {};

    std::string body(len, '\0');
    int n = FCGX_GetStr(body.data(), static_cast<int>(len), r.in);
    body.resize(n > 0 ? n : 0);
    return body;
}

static const char* status_text(int s) {
    switch (s) {
        case 200: return "OK";
        case 202: return "Accepted";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        default:  return "Internal Server Error";
    }
}

int main(int argc, char *argv[]) {

    HttpRoute router;
    router.add("POST", "/BTI/v1.0/collector", EventHandler::publish);

	FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {

        Request req;

        req.query_string	    =	FCGX_GetParam("QUERY_STRING", request.envp);
        req.method     		    =	FCGX_GetParam("REQUEST_METHOD", request.envp);
        req.content_type	    =	FCGX_GetParam("CONTENT_TYPE", request.envp);
        req.content_length		=	FCGX_GetParam("CONTENT_LENGTH", request.envp);
        req.script_name		    =	FCGX_GetParam("SCRIPT_NAME", request.envp);
        req.uri    			    =	FCGX_GetParam("REQUEST_URI", request.envp);
        req.document_uri	   	=	FCGX_GetParam("DOCUMENT_URI", request.envp);
        req.document_root 	   	=	FCGX_GetParam("DOCUMENT_ROOT", request.envp);
        req.server_protocol	    =	FCGX_GetParam("SERVER_PROTOCOL", request.envp);
        req.scheme     		    =	FCGX_GetParam("REQUEST_SCHEME", request.envp);
        req.gateway_interface   =	FCGX_GetParam("GATEWAY_INTERFACE", request.envp);
        req.server_software	    =	FCGX_GetParam("SERVER_SOFTWARE", request.envp);
        req.remote_addr		    =	FCGX_GetParam("REMOTE_ADDR", request.envp);
        req.remote_port		    =	FCGX_GetParam("REMOTE_PORT", request.envp);
        req.server_addr		    =	FCGX_GetParam("SERVER_ADDR", request.envp);
        req.server_port		    =	FCGX_GetParam("SERVER_PORT", request.envp);
        req.server_name		    =	FCGX_GetParam("SERVER_NAME", request.envp);
        req.server_version      =   FCGX_GetParam("SERVER_VERSION", request.envp);
        req.bti_manufacturer    =   FCGX_GetParam("BTI_MANUFACTURER", request.envp);
        req.bti_product         =   FCGX_GetParam("BTI_PRODUCT", request.envp);
        req.bti_version         =   FCGX_GetParam("BTI_VERSION", request.envp);
        req.bti_component       =   FCGX_GetParam("BTI_COMPONENT", request.envp);

        Response res;
        try {
            router.dispatch(req, res);
        }
        catch (const std::exception& e) {
            res.status = 500;
            res.body = R"({"error":"internal error"})";
        }

        FCGX_FPrintF(request.out,
            "Status: %d %s\r\nContent-Type: %s\r\n\r\n",
            res.status, status_text(res.status),
            res.content_type.c_str());
        FCGX_PutStr(res.body.data(), static_cast<int>(res.body.size()), request.out);

        FCGX_Finish_r(&request);
    }

    return 0;
}