#include <algorithm>
#include <cctype>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string_view>
#include <types.h>
#include <unordered_map>
#include <logger.h>
#include <string>
#include <optional>
#include <format>
#include <utility>
#pragma once
namespace ff {
    namespace http {
        extern const std::string EOL;
        enum class command {
            NONE,
            GET
        };

        namespace {
            using cmdStrMap_t = std::unordered_map<command, std::string>;
            using strCmdMap_t = std::unordered_map<std::string, command>;

            const strCmdMap_t cmdStrMapConversion(const cmdStrMap_t map) 
                {
                    strCmdMap_t ret;
                    auto f = [&ret](auto it) {
                    ret.emplace(it.second, it.first);
                };
                std::for_each(map.begin(), map.end(), f);
                return ret;
            }

            inline const cmdStrMap_t cmdStrMap{
                {command::NONE, "NONE"},
                    {command::GET, "GET"}
            };

            inline const strCmdMap_t strCmdMap = cmdStrMapConversion(cmdStrMap);

            inline const u16 noRespCode{0};
            inline const std::string defaultUrl{};

            enum class messageState {
                EMPTY,
                STARTDONE,
                HEADERSDONE,
                BODYDONE
            };

            std::array<std::string, 3> splitStartLine(const std::string s) {
                std::array<std::string, 3> ret;
                size_t begin = 0;
                size_t end = s.find(" ", begin);
                ret[0] = s.substr(begin, end - begin);
                begin = end + 1;
                end = s.find(" ", begin);
                ret[1] = s.substr(begin, end - begin);
                begin = end + 1;
                ret[2] = s.substr(begin);
                return ret;
            }
        }

        struct startLine {
            command command;
            std::string url;
            u8 version{11};
            u16 respCode;
            std::string reason;

            startLine() :
                command{command::NONE}, url{""}, respCode{noRespCode}, reason{}
            {}

            startLine(const ::ff::http::command command,
                    const std::string url,
                    const u16 respCode = noRespCode, 
                    const std::string reason = "") :
                command(command),
                url(url),
                respCode(respCode),
                reason(reason)
                {}

            std::string string() const;
            bool empty() const;
        };

        struct headers {
            std::unordered_map<std::string, std::string> values;

            std::string string() const;
            bool empty() const {
                return values.empty();
            }
            headers() = default;
            headers(std::initializer_list<
                    std::pair<const std::string, std::string>
                    > v)
                : values(v)
            {}
        };

        using body = std::string;
        // readonly struct
        struct message {
            private: 
                messageState state = messageState::EMPTY;;
                uint32_t bodySizeLeft = 0;
            public:
                startLine startLine;
                headers headers;
                body body;

                message() = default;
                message(struct startLine s, struct headers h, ff::http::body b) :
                    startLine(s), headers(h), body(b)
            {}
                
                std::string string() const;

                // returns message and optional leftover bytes
                using ostring = std::optional<std::string>;
                [[nodiscard("Optional bytes may be available")]]
                    ostring modify(std::string rawBytes);

                bool complete() const;

            private:
                void parseStartLine(const std::string currLine);

                bool parseHeaders(const std::string currLine);

                bool parseBody(std::string currLine);

                void trimEOL(std::string& msg);

                void trimWhiteSpace(std::string& msg);
        };

        struct get : message {
            using message::message;
            using message::startLine;
            get(const std::string url, const ::ff::http::headers headers, const ::ff::http::body body) :
                message{{::ff::http::command::GET, url}, headers, body}
            {}
        };

        struct response : message {
            using message::message;
            response(const u16 statusCode, const std::string statusMessage, 
                    const ::ff::http::headers headers, const ::ff::http::body body) :
                message{
                    {::ff::http::command::NONE, defaultUrl, statusCode, statusMessage},
                    headers, body}
            {}
        };
    }
}
// model usage
// int main() {
//     httpClientSocket s{url};
//     httpGet req{url, headers, body}
//     s.send(httpGet);
//         

//     // server
//     httpServerSocket s{url};
//     httpCommand c = s.receive();
//     if (c.method == GET) {
//         httpResp resp{"403", "forbidden", headers, body}
//         s.send(resp);
//     }
// }
