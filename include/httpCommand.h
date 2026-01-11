#include <algorithm>
#include <string_view>
#include <types.h>
#include <unordered_map>
#include <map>
#include <string>
#include <format>
namespace ff {
    namespace http {
        enum class command {
            NONE,
            // POST,
            // PUT,
            // DELETE,
            GET
        };

        namespace {
            const std::map<command, std::string> cmdStrMap{
                {command::NONE, "NONE"},
                    {command::GET, "GET"}
            };

            const u8 noRespCode{0};
            using std::operator""sv;
            const std::string defaultUrl{};
        }

        struct startLine {
            const command command;
            const std::string url;
            const u8 version{11};
            const u8 respCode;
            const std::string reason;

            startLine() :
                command{command::NONE}, url{""}, respCode{noRespCode}, reason{}
            {}

            startLine(const ::ff::http::command command,
                    const std::string url,
                    const u8 respCode = noRespCode, 
                    const std::string reason = "");

            // convert startline to valid http string \r\n terminated
            std::string string() const {
                std::string ret;
                if (command != ::ff::http::command::NONE) {
                    ret = std::vformat("{} {} ",
                            std::make_format_args(cmdStrMap.at(command), url));
                }
                const u8 vMajor = version / 10;
                const u8 vMinor = version % 10;
                ret += std::vformat("HTTP/{}.{}",
                        std::make_format_args(vMajor, vMinor));
                if (respCode != noRespCode) {
                    ret += std::vformat("{} {}",
                            std::make_format_args(respCode, reason));
                }
                ret += "\r\n";
                return ret;
            }
        };

        struct headers {
            std::unordered_map<std::string, std::string> values;
            std::string string() const {
                std::string ret;
                auto func = [&ret](const std::pair<std::string, std::string> p){
                    ret = ret + p.first + ":" + p.second + "\r\n";
                };

                std::for_each(values.begin(), values.end(), func);
                return ret;
            }
        };

        using body = std::string;

        // readonly struct
        struct message {
            const startLine startLine;
            const headers headers;
            const body body;
            std::string string() const {
                return std::string{startLine.string() +
                    headers.string() + "\r\n" + 
                    body + "\r\n"};
            }
        };

        struct get : message {
            using message::message;
            get(const std::string url, const ::ff::http::headers headers, const ::ff::http::body body) :
                message{{::ff::http::command::GET, url}, headers, body}
                {}
        };

        struct response : message {
            using message::message;
            response(const u8 statusCode, const std::string statusMessage, 
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
