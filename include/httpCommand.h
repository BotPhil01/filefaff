#include <algorithm>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <types.h>
#include <unordered_map>
#include <logger.h>
#include <string>
#include <optional>
#include <format>
#include <utility>
namespace ff {
    namespace http {
        const std::string EOL{"\r\n"};
        enum class command {
            NONE,
            // POST,
            // PUT,
            // DELETE,
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

            const cmdStrMap_t cmdStrMap{
                {command::NONE, "NONE"},
                    {command::GET, "GET"}
            };

            const strCmdMap_t strCmdMap = cmdStrMapConversion(cmdStrMap);

            const u16 noRespCode{0};
            const std::string defaultUrl{};

            enum class messageState {
                EMPTY,
                STARTDONE,
                HEADERSDONE,
                BODYDONE
            };
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

            bool empty() const {
                if (command == command::NONE && (!url.empty() || !reason.empty() 
                        || respCode != noRespCode)) {
                    throw std::runtime_error("Invalid startline found with partial config");
                }
                return command == command::NONE;
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
            bool empty() const {
                return values.empty();
            }
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
                message(struct startLine s, struct headers h, ff::http::body b);
                
                std::string string() const {
                    return std::string{startLine.string() +
                        headers.string() + EOL + 
                            body + EOL};
                }
                // returns message and optional leftover bytes
                using ostring = std::optional<std::string>;
                [[nodiscard("Optional bytes may be available")]]
                    ostring modify(std::string rawBytes) 
                {
                    bool headersSet;
                    while (rawBytes.contains(EOL)) 
                    {
                        const std::size_t splitIndex = rawBytes.find(EOL);
                        const std::string currLine = rawBytes.substr(0, splitIndex);
                        rawBytes = rawBytes.substr(splitIndex + EOL.size());
                        LOG << "choosing parser\n";
                        switch (state) {
                            case (messageState::EMPTY):
                                parseStartLine(currLine);
                                state = messageState::STARTDONE;
                                break;
                            case (messageState::STARTDONE):
                                if(parseHeaders(currLine)) {
                                    state = messageState::HEADERSDONE;
                                }
                                break;
                            case (messageState::HEADERSDONE):
                                parseBody(currLine);
                                break;
                            default:
                                throw std::runtime_error("Invalid state reached");
                        }
                        LOG << "\n";
                    }
                    if (rawBytes.empty()) {
                        return {};
                    }
                    return rawBytes;
                }
            private:
                void parseStartLine(const std::string currLine) 
                {
                    LOG << "parsing startLine " << currLine << "\r\n";
                    // parse currLine
                    if (currLine.starts_with("HTTP")) 
                    {
                        // response
                        std::size_t subSplitIndex{currLine.find(' ')};
                        const std::string version{
                            currLine.substr(0, subSplitIndex)
                        };
                        subSplitIndex = currLine.find(' ', subSplitIndex + 1);
                        size_t beginIndex{version.size() + 1};
                        const std::string statusCode{
                            currLine.substr(beginIndex, subSplitIndex - beginIndex)};

                        const size_t eolIndex{currLine.find(EOL)};
                        beginIndex = subSplitIndex + 1;
                        const std::string reason{
                            currLine.substr(beginIndex, eolIndex - beginIndex)
                        };
                        if (!version.contains("1.1")) {
                            LOG << "ERROR " << version << "\n";
                            throw std::runtime_error(
                                    "HTTP version not supported");
                        }

                        startLine.respCode = static_cast<u16>(stoi(statusCode));
                        startLine.reason = reason;
                    } else 
                    {
                        // request
                        std::size_t subSplitIndex{currLine.find(' ')};
                        const std::string command{
                            currLine.substr(0, subSplitIndex + 1)
                        };
                        subSplitIndex = currLine.find(' ', subSplitIndex);
                        size_t beginIndex{command.size() + 1};
                        const std::string url{
                            currLine.substr(beginIndex, subSplitIndex - beginIndex)
                        };

                        const size_t eolIndex{currLine.find(EOL)};
                        beginIndex = subSplitIndex + 1;
                        const std::string version{
                            currLine.substr(beginIndex, eolIndex - beginIndex)
                        };

                        if (!version.contains("1.1")) {
                            // this is unsafe
                            LOG << "ERROR " << version << "\n";
                            throw std::runtime_error(
                                    "HTTP version not supported");
                        }

                        startLine.command = strCmdMap.at(command);
                        startLine.url = url;
                    }
                }

                bool parseHeaders(const std::string currLine) 
                {
                    LOG << "parsing headers " << currLine << "\r\n";
                    if (!currLine.contains(":")) {
                        if (currLine == "") {
                            return true;
                        }
                        throw std::runtime_error("Invalid header being parsed");

                    }
                    const size_t splitIndex = currLine.find(":");
                    std::string key = currLine.substr(0, splitIndex);
                    std::string value = currLine.substr(splitIndex+1);
                    
                    // trim whitespace from key and value
                    trimWhiteSpace(key);
                    trimWhiteSpace(value);
                    if (key == "Content-Length") {
                        bodySizeLeft = static_cast<uint32_t>(stoi(value));
                    }

                    headers.values.emplace(key, value);
                    return false;
                }

                void parseBody(std::string currLine) 
                {
                    LOG << "parsing body {" << currLine << "}\n";
                    if (currLine == "" && bodySizeLeft != 0) {
                        throw std::runtime_error("body ended but more expected");
                    }

                    currLine = currLine.substr(0, currLine.find("\r\n"));
                    if (currLine.size() > bodySizeLeft) {
                        throw std::runtime_error("body line too big");
                    }
                    body += currLine;
                    bodySizeLeft = bodySizeLeft - currLine.size();
                }

                void trimEOL(std::string& msg) {
                    LOG << "trimEOL: msg {" << msg << "} new {";
                    const size_t EOLIndex = msg.find(EOL);
                    msg = msg.substr(0, EOLIndex);
                    LOG << msg << "}\n";
                }
                void trimWhiteSpace(std::string& msg) 
                {
                    LOG << "trimWhiteSpace msg{" << msg << "} new {";
                    auto f = [](unsigned char it) {
                        return !std::isspace(it);
                    };
                    msg.erase(msg.begin(), std::find_if(msg.begin(), msg.end(), f));
                    auto rbeg = msg.rbegin()++;
                    const auto original = rbeg;
                    while (*rbeg == ' ') {
                        rbeg++;
                    }
                    using itertype = decltype(rbeg);
                    // todo fix this
                    msg = msg.substr(0, msg.size() - std::distance(original, rbeg));

                    LOG << msg << "}\n";

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
