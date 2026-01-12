#include <cassert>
#include <iostream>
#include <logger.h>
#include <httpCommand.h>

int main() {
    LOG << "Log test\n";

    ff::http::message cmd;
    const std::string bytes = 
        "HTTP/1.1 200 Success\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection:Keep-Alive\r\n"
        "Content-Length:7\r\n"
        "\r\n"
        "content\r\n"
        ;
    assert(!cmd.modify(bytes));
    assert(cmd.startLine.command == ff::http::command::NONE);
    assert(cmd.startLine.url == "");
    assert(cmd.startLine.respCode == 200);
    assert(cmd.startLine.version == 11);
    assert(cmd.startLine.reason == "Success");
}
