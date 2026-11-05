// HTTP router for BTI support.
//
// Copyright (C) 2026 Daniel Herrera Cabrera <dani_goo@hotmail.com>
#ifndef _BTI_HTTP_ROUTE
#define _BTI_HTTP_ROUTE

#include "http.hpp"
#include <string_view>
#include <string.h>
#include <functional>
#include <sstream>
#include <vector>
#include "fcgio.h"

using namespace std;

namespace novasoft_bti_base {

    using Handler = std::function<void(const Request&, Response&)>;

    class HttpRoute {

        public:

            void add(std::string method, std::string pattern, Handler h) {
                routes_.push_back({std::move(method), split(pattern), std::move(h)});
            }

            void dispatch(Request& request, Response& response) const {

                auto segments = split(request.document_uri);

                bool path_matched = false;
                for (const auto& r : routes_) {

                    std::unordered_map<std::string, std::string> params;

                    if (!match(r.segments, segments, params)) continue;

                    path_matched = true;
                    if (r.method != request.method) continue;

                    request.params = std::move(params);
                    r.handler(request, response);
                    return;
                }

                if (path_matched) {
                    response.status = 405;
                    response.body = R"({"error":"method not allowed"})";
                } else {
                    response.status = 404;
                    response.body = R"({"error":"not found"})";
                }
            }

        private:

            struct Route {
                std::string method;
                std::vector<std::string> segments;
                Handler handler;
            };

            std::vector<Route> routes_;

            static std::vector<std::string> split(const std::string & path) {
                std::vector<std::string> out;
                std::stringstream ss(path);
                std::string seg;
                while (std::getline(ss, seg, '/'))
                    if (!seg.empty()) out.push_back(seg);
                return out;
            }

            static bool match(const std::vector<std::string>& pat, const std::vector<std::string>& in, std::unordered_map<std::string, std::string>& params) {
                if (pat.size() != in.size()) return false;

                for (size_t i = 0; i < pat.size(); ++i) {
                    const auto& p = pat[i];
                    if (p.size() > 2 && p.front() == '{' && p.back() == '}')
                        params[p.substr(1, p.size() - 2)] = in[i];
                    else if (p != in[i])
                        return false;
                }

                return true;
            }

    };

}

#endif /* _BTI_HTTP_ROUTE */