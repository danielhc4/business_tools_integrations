// HTTP response codes for BTI support.
//
// Copyright (C) 2026 Daniel Herrera Cabrera <dani_goo@hotmail.com>
#ifndef _BTI_Utils
#define _BTI_Utils

#include <string_view>
#include <string.h>

using namespace std;

namespace BTICollectorUtils {

    class BTI_Utils {
        public:
            static string sanitize_params(const std::string& params);
            static bool file_exists(const std::string& name);
    };

}

#endif /* _BTI_Utils */