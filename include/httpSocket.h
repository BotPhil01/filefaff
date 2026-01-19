#include <raiiSocket.h>
#include <httpCommand.h>
#include <format>
#include <functional>

#pragma once
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
            {
                connect();
            }

                virtual void send(const ff::http::message httpMsg) override;

                virtual ff::http::message receive() override;

        };

        class serverSocket : private ::ff::http::socket, private serverRaiiSocket
        {
            public:
                using serverRaiiSocket::serverRaiiSocket;
                serverSocket(const u16 port) :
                    serverRaiiSocket(port) 
            {
                bind();
                listen(1);
                accept();
            }
                virtual void send(const ff::http::message httpMsg) override;
                virtual ff::http::message receive() override;
        };
    }
}
