#include <raiiSocket.h>
#include <httpCommand.h>
#include <httpSocket.h>

void ff::http::clientSocket::send(const ff::http::message httpMsg) 
{
    clientRaiiSocket::write(httpMsg.string());
}
ff::http::message ff::http::clientSocket::receive()
{
    // read line
    // process line
    message m;
    std::string leftOver;
    while (true) {
        const std::string bytes =
            leftOver + clientRaiiSocket::read();
        leftOver = m.modify(bytes).value_or("");
        if (m.complete()) {
            break;
        }
    }
    return m;
}
void ff::http::serverSocket::send(const ff::http::message httpMsg)
{
    LOG << "writing msg " << httpMsg.string() << "to socket\n";
    serverRaiiSocket::write(httpMsg.string());
}

ff::http::message ff::http::serverSocket::receive()
{
    // read line
    // process line
    message m;
    std::string leftOver;
    while (true) {
        const std::string bytes =
            leftOver + serverRaiiSocket::read();
        leftOver = m.modify(bytes).value_or("");
        if (m.complete()) {
            break;
        }
    }
    return m;
}

