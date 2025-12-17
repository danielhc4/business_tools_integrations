#ifndef _BSM_UTILS
#define _BSM_UTILS

#include <string_view>
#include <string.h>

using namespace std;

namespace BSMCollectorUtils {

    class BSM_Utils {
        public:
            static string sanitize_params(const std::string& params);
            static bool file_exists(const std::string& name);
    };

}

#endif /* _BSM_UTILS */