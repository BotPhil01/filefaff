#include "raiiSocket.h"
#include <cerrno>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <iostream>
#include <types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ports.h>
#include <httpCommand.h>
#include <httpSocket.h>
#include <resourceLoader.h>

i32 main() {
    ff::http::serverSocket s{PORT};
    ff::http::message m = s.receive();
    std::cout << m.string();

    try {
        const std::string body = ff::http::getResource(m.startLine.url);
        const std::string bodySize = std::to_string(body.size());
        const ff::http::headers h{
            {"Content-Length", bodySize},
                {"abc", "123"}
        };
        ff::http::response r{200, "OK", h, body};
        s.send(r);
    }
    catch (std::domain_error e) {
        ff::http::response r{401, "Unauthorized", {{"Content-Length", "0"}}, ""};
        s.send(r);
    }
    catch (std::runtime_error e) {
        ff::http::response r{404, "Not Found", {{"Content-Length", "0"}}, ""};
        s.send(r);
    }
}
