#include <iostream>
#include "fcgio.h"
#include "http_server_api.hpp"

using namespace std;
using namespace novasoft_bti_base;

int main(int argc, char *argv[]) {

	FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        HttpApi::router(request);
    }

    return 0;
}