#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>

const size_t BUF_SIZE = 1024;
const int fdwrite(const int sockfd, std::ifstream &stream) {
    std::string buffer(BUF_SIZE, 0);
    while (true) {
        stream.read(&buffer[0], BUF_SIZE);
        const ssize_t writesize = write(sockfd, &buffer[0], stream.gcount());
        if (stream.gcount() < BUF_SIZE) {
            return 0;
        }

        if (writesize != stream.gcount()) {
            std::cerr << "Insufficient bytes written to socket " << errno << std::endl;
            return 1;
        }
    }
}
