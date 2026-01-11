#include "socket.h"
#include <cerrno>
#include <unistd.h>
#include <iostream>
#include <types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ports.h>

// socket identified via integer hence can pass it around like nobody's business

i32 main() {
    ff::serverSocket s(port);
    s.bind();
    std::cout << "bound" << std::endl;
    s.listen(1);
    std::cout << "listening" << std::endl;
    s.accept();
    std::cout << "accepted" << std::endl;
    std::cout << s.read() << std::endl;
}
