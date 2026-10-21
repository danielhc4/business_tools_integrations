#include <string_view>
#include <string>
#include <sys/stat.h>

#include "../../include/http_server/http_server_utils.hpp"

using namespace std;

namespace BTICollectorUtils {

    string BTI_Utils::sanitize_params(const std::string& params) {
        string s_param = params;
        if(s_param.find("\"", 0) != std::string::npos) {
            s_param.erase(0, 1);
        }
        if(s_param.find("\"", s_param.length() - 1, 1) != std::string::npos) {
            s_param.erase(s_param.length() - 1, 1);
        }
        return s_param;
    }

    bool BTI_Utils::file_exists(const std::string& name) {
        struct stat buffer;
        return (stat (name.c_str(), &buffer) == 0); 
    }

}