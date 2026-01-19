#include <httpCommand.h>
#include <algorithm>

namespace ff::http {
        const std::string EOL{"\r\n"};
        std::string startLine::string() const 
        {
            std::string ret;
            if (command != ::ff::http::command::NONE) {
                ret = std::vformat("{} {} ",
                        std::make_format_args(cmdStrMap.at(command), url));
            }
            const u8 vMajor = version / 10;
            const u8 vMinor = version % 10;
            ret += std::vformat("HTTP/{}.{} ",
                    std::make_format_args(vMajor, vMinor));
            if (respCode != noRespCode) {
                ret += std::vformat("{} {}",
                        std::make_format_args(respCode, reason));
            }
            ret += "\r\n";
            return ret;
        }
        bool startLine::empty() const 
        {
            if (command == command::NONE && (!url.empty() || !reason.empty() 
                        || respCode != noRespCode)) {
                throw std::runtime_error("Invalid startline found with partial config");
            }
            return command == command::NONE;
        }

        std::string headers::string() const 
        {
            std::string ret;
            auto func = [&ret](const std::pair<std::string, std::string> p){
                ret = ret + p.first + ":" + p.second + "\r\n";
            };

            std::for_each(values.begin(), values.end(), func);
            return ret;
        }

        std::string message::string() const 
        {
            return std::string{startLine.string() +
                headers.string() + EOL + 
                    body + EOL};
        }

        // returns message and optional leftover bytes
        using ostring = std::optional<std::string>;
        [[nodiscard("Optional bytes may be available")]]
            ostring message::modify(std::string rawBytes) 
            {
                bool headersSet;
                while (rawBytes.contains(EOL) || 
                        (state == messageState::HEADERSDONE && !rawBytes.empty())) 
                {
                    std::string currLine;
                    if (state == messageState::HEADERSDONE) {
                        currLine = rawBytes;
                        rawBytes.clear();
                    } else {
                        const std::size_t splitIndex = rawBytes.find(EOL);
                        currLine = rawBytes.substr(0, splitIndex);
                        rawBytes = rawBytes.substr(splitIndex + EOL.size());
                    }

                    LOG << "choosing parser\n";
                    switch (state) {
                        case (messageState::EMPTY):
                            parseStartLine(currLine);
                            state = messageState::STARTDONE;
                            break;
                        case (messageState::STARTDONE):
                            if (parseHeaders(currLine)) {
                                state = messageState::HEADERSDONE;
                            }
                            break;
                        case (messageState::HEADERSDONE):
                            if (parseBody(currLine)) {
                                state = messageState::BODYDONE;
                            }
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

        bool message::complete() const {
            return state == messageState::BODYDONE;
        }
        void message::parseStartLine(const std::string currLine) 
        {
            LOG << "parsing startLine " << currLine << "\r\n";
            // parse currLine
            const std::array<std::string, 3> parts{splitStartLine(currLine)};
            if (parts.at(0).starts_with("HTTP")) 
            {
                // HTTP/1.1 200 OK
                if (const std::string version = parts.at(0);
                        !version.contains("1.1")) {
                    LOG << "ERROR " << version << "\n";
                    throw std::runtime_error(
                            "HTTP version not supported");
                }

                startLine.respCode = static_cast<u16>(stoi(parts.at(1)));
                startLine.reason = parts.at(2);
            } else 
            {
                // GET / HTTP/1.1\r\n
                if (const std::string version = parts.at(2);
                        !version.contains("1.1")) {
                    // this is unsafe
                    LOG << "ERROR " << version << "\n";
                    throw std::runtime_error(
                            "HTTP version not supported");
                }

                startLine.command = strCmdMap.at(parts.at(0));
                startLine.url = parts.at(1);
            }
        }

        bool message::parseHeaders(const std::string currLine) 
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

        bool message::parseBody(std::string currLine) 
        {
            LOG << "parsing body {" << currLine << "}\n";
            if (currLine == "" && bodySizeLeft != 0) {
                throw std::runtime_error("body ended but more expected");
            }

            const size_t parseAmount = std::min(bodySizeLeft, static_cast<std::uint32_t>(currLine.size()));
            
            body += currLine.substr(0, parseAmount);
            bodySizeLeft = bodySizeLeft - parseAmount;
            return bodySizeLeft == 0;
        }

        void message::trimEOL(std::string& msg) {
            LOG << "trimEOL: msg {" << msg << "} new {";
            const size_t EOLIndex = msg.find(EOL);
            msg = msg.substr(0, EOLIndex);
            LOG << msg << "}\n";
        }

        void message::trimWhiteSpace(std::string& msg) 
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
            msg = msg.substr(0, msg.size() - std::distance(original, rbeg));
            LOG << msg << "}\n";
        }
}
