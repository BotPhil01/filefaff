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
                    // read line
                    // process line
                    message m;
                    std::string leftOver;
                    while (true) {
                        const std::string bytes =
                            clientRaiiSocket::read() + leftOver;
                        leftOver = m.modify(bytes).value_or("");
                        if (bytes.size() != clientRaiiSocket::READBUFSIZE) {
                            break;
                        }
                    }
                    return m;
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
