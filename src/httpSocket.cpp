#include <raiiSocket.h>
#include <httpCommand.h>
#include <format>
#include <functional>

namespace ff
{ 
    namespace http
    {
        namespace
        {
            class socket
            {
                public:
                    virtual void send(ff::http::message httpMsg) = 0;
                    virtual ff::http::message receive() = 0;
            };
            struct startLine processStartLine(std::string s)
            {
                int index = s.find_first_of(" ");
                std::string part1 = s.substr(0, index);

                s = s.substr(index);
                index = s.find_first_of(" ");
                std::string part2 = s.substr(0, index);

                s = s.substr(index);
                index = s.find_first_of(" ");
                std::string part3 = s.substr(0, index);

                if (part1.starts_with("HTTP")) {
                    // response
                } else {
                    // request
                }
            }
        }

        class clientSocket : private ::ff::http::socket, private clientRaiiSocket
        {
            // line based socket reading
            // delimiter is \r\n
            // read 2 chars at a time?
            private:
            public:
                clientSocket(const u16 port) :
                    clientRaiiSocket::clientRaiiSocket(port) 

            {}

                virtual void send(const ff::http::message httpMsg) override
                {
                    clientRaiiSocket::write(httpMsg.string());
                }

                virtual ff::http::message receive() override
                {
                    struct startLine start;
                    headers headers;
                    uint8_t progress = 0;
                    std::string oldBytes;
                    bool moreLines = false;
                    while (true) 
                    {
                        std::string bytes;
                        if (moreLines) 
                        {
                            bytes = oldBytes;
                        }
                        else
                        {
                            bytes = clientRaiiSocket::read();
                        }
                        // need state machine
                        // and to extract conent length from headers when read
                        size_t index = bytes.find("\r\n");
                        if (index == bytes.npos) 
                        {
                            oldBytes += bytes;
                            continue;
                        }
                        std::string line = bytes.substr(0, index) + oldBytes;
                        oldBytes = bytes.substr(index);
                        if (0 == progress) 
                        {
                            start = processStartLine(line);
                            progress++;
                        }
                        else if (1 == progress)
                        {
                            if (!line.compare("\r\n")) 
                            {
                                headers.values.emplace(processHeader(line);
                            }
                            else 
                            {
                            progress++;
                            }
                        }
                        if (oldBytes.find("\r\n") != oldBytes.npos) 
                        {
                            moreLines = true;
                        } 
                        else 
                        {
                            moreLines = false;
                        }
                    }
                }
        };

        class serverSocket : private ::ff::http::socket, private serverRaiiSocket
        {
            private:
            public:
                using serverRaiiSocket::serverRaiiSocket;
                serverSocket(const u16 port) :
                    serverRaiiSocket(port) 

            {}
                virtual void send(const ff::http::message httpMsg) override
                {
                    serverRaiiSocket::write(httpMsg.string());
                }
        };
    }
}
