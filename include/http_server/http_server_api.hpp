// HTTP response codes for BTI support.
//
// Copyright (C) 2023 Daniel Herrera Cabrera <dani_goo@hotmail.com>

#ifndef _BTI_HTTP
#define _BTI_HTTP

#include <string_view>
#include "fcgio.h"

// Information responses
// 100
#define _BTI_HTTP_100 "Status: 100 Continue \r\n\r\n"
#define _BTI_HTTP_101 "Status: 101 Switching Protocols \r\n\r\n"
#define _BTI_HTTP_102 "Status: 102 Processing \r\n\r\n"
#define _BTI_HTTP_103 "Status: 103 Early Hints \r\n\r\n"

// Successful responses
// 200
#define _BTI_HTTP_200 "Status: 200 OK \r\n\r\n"
#define _BTI_HTTP_201 "Status: 201 Created \r\n\r\n"
#define _BTI_HTTP_202 "Status: 202 Accepted \r\n\r\n"
#define _BTI_HTTP_203 "Status: 203 Non-Authoritative Information \r\n\r\n"
#define _BTI_HTTP_204 "Status: 204 No Content \r\n\r\n"
#define _BTI_HTTP_205 "Status: 205 Reset Content \r\n\r\n"
#define _BTI_HTTP_206 "Status: 206 Partial Content \r\n\r\n"
#define _BTI_HTTP_207 "Status: 207 Multi-Status \r\n\r\n"
#define _BTI_HTTP_208 "Status: 208 Already Reported \r\n\r\n"
#define _BTI_HTTP_226 "Status: 226 IM Used \r\n\r\n"

// Redirection messages
// 300
#define _BTI_HTTP_300 "Status: 300 Multiple Choices \r\n\r\n"
#define _BTI_HTTP_301 "Status: 301 Moved Permanently \r\n\r\n"
#define _BTI_HTTP_302 "Status: 302 Found \r\n\r\n"
#define _BTI_HTTP_303 "Status: 303 See Other \r\n\r\n"
#define _BTI_HTTP_304 "Status: 304 Not Modified \r\n\r\n"
#define _BTI_HTTP_305 "Status: 305 Use Proxy \r\n\r\n"
#define _BTI_HTTP_307 "Status: 307 Temporary Redirect \r\n\r\n"
#define _BTI_HTTP_308 "Status: 308 Permanent Redirect \r\n\r\n"

// Client error responses
// 400
#define _BTI_HTTP_400 "Status: 400 Bad Request \r\n\r\n"
#define _BTI_HTTP_401 "Status: 401 Unauthorized \r\n\r\n"
#define _BTI_HTTP_402 "Status: 402 Payment Required \r\n\r\n"
#define _BTI_HTTP_403 "Status: 403 Forbidden \r\n\r\n"
#define _BTI_HTTP_404 "Status: 404 Not Found \r\n\r\n"
#define _BTI_HTTP_405 "Status: 405 Method Not Allowed \r\n\r\n"
#define _BTI_HTTP_406 "Status: 406 Not Acceptable \r\n\r\n"
#define _BTI_HTTP_407 "Status: 407 Proxy Authentication Required \r\n\r\n"
#define _BTI_HTTP_408 "Status: 408 Request Timeout \r\n\r\n"
#define _BTI_HTTP_409 "Status: 409 Conflict \r\n\r\n"
#define _BTI_HTTP_410 "Status: 410 Gone \r\n\r\n"
#define _BTI_HTTP_411 "Status: 411 Length Required \r\n\r\n"
#define _BTI_HTTP_412 "Status: 412 Precondition Failed \r\n\r\n"
#define _BTI_HTTP_413 "Status: 413 Payload Too Large \r\n\r\n"
#define _BTI_HTTP_414 "Status: 414 URI Too Long \r\n\r\n"
#define _BTI_HTTP_415 "Status: 415 Unsupported Media Type \r\n\r\n"
#define _BTI_HTTP_416 "Status: 416 Range Not Satisfiable \r\n\r\n"
#define _BTI_HTTP_417 "Status: 417 Expectation Failed \r\n\r\n"
#define _BTI_HTTP_418 "Status: 418 I'm a teapot \r\n\r\n"
#define _BTI_HTTP_421 "Status: 421 Misdirected Request \r\n\r\n"
#define _BTI_HTTP_422 "Status: 422 Unprocessable Content \r\n\r\n"
#define _BTI_HTTP_423 "Status: 423 Locked \r\n\r\n"
#define _BTI_HTTP_424 "Status: 424 Failed Dependency \r\n\r\n"
#define _BTI_HTTP_425 "Status: 425 Too Early \r\n\r\n"
#define _BTI_HTTP_426 "Status: 426 Upgrade Required \r\n\r\n"
#define _BTI_HTTP_428 "Status: 428 Precondition Required \r\n\r\n"
#define _BTI_HTTP_429 "Status: 429 Too Many Requests \r\n\r\n"
#define _BTI_HTTP_431 "Status: 431 Request Header Fields Too Large \r\n\r\n"
#define _BTI_HTTP_451 "Status: 451 Unavailable For Legal Reasons \r\n\r\n"

// Server error responses
// 500
#define _BTI_HTTP_500 "Status: 500 Internal Server Error \r\n\r\n"
#define _BTI_HTTP_501 "Status: 501 Not Implemented \r\n\r\n"
#define _BTI_HTTP_502 "Status: 502 Bad Gateway \r\n\r\n"
#define _BTI_HTTP_503 "Status: 503 Service Unavailable \r\n\r\n"
#define _BTI_HTTP_504 "Status: 504 Gateway Timeout \r\n\r\n"
#define _BTI_HTTP_505 "Status: 505 HTTP Version Not Supported \r\n\r\n"
#define _BTI_HTTP_506 "Status: 506 Variant Also Negotiates \r\n\r\n"
#define _BTI_HTTP_507 "Status: 507 Insufficient Storage \r\n\r\n"
#define _BTI_HTTP_508 "Status: 508 Loop Detected \r\n\r\n"
#define _BTI_HTTP_510 "Status: 510 Not Extended \r\n\r\n"
#define _BTI_HTTP_511 "Status: 511 Network Authentication Required \r\n\r\n"

using namespace std;

namespace novasoft_bti_base {

    class HttpApi {

        public:

            enum class HTTP_VERBS {
                GET,
                HEAD,
                POST,
                PUT,
                PATCH,
                DELETE,
                OPTIONS,
                NOT_ALLOWED
            };

            static HTTP_VERBS method_requested(char *method);

            static string get_body_request();

            static void GET_routes();
            static void POST_routes();
            static void PUT_routes();
            static void PATCH_routes();
            static void DELETE_routes();
            static void HEAD_routes();
            static void OPTIONS_routes();

            static void code_404();
            static void code_405();

            static void router(FCGX_Request &request);

    };

}

#endif /* _BTI_HTTP */