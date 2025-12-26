#include <cerrno>
#include <unistd.h>
#include <iostream>
#include <types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;
const u16 PORT = 42069;
// socket identified via integer hence can pass it around like nobody's business

i32 main() {
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    const uint16_t netport = htons(PORT);
    cout << "netport " << netport << endl;
    sockaddr_in address { 
        .sin_family = AF_INET,
        .sin_port = netport,
        .sin_addr = INADDR_ANY };
    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address))) {
        cerr << "Error while binding to address " << errno << endl;
        // cerr << "address " << address.<< endl;
        close(sockfd);
        return 1;
    }
    if(listen(sockfd, 1)) {
        cerr << "Error while listening for connections " << errno << endl;
        close(sockfd);
        return 1;
    }
    const int clientfd = accept(sockfd, nullptr, nullptr);
    const size_t BUF_SIZE = 1000000;
    string buffer(BUF_SIZE, 0);
    const ssize_t readret = read(clientfd, &buffer[0], BUF_SIZE);
    if (readret == -1) {
        cerr << "Error reading from socket " << errno << endl;
    }
    cout << "Message: " << buffer << endl;
    if (shutdown(sockfd, SHUT_RDWR)) {
        cerr << "Error shutting down file descriptor " << errno << endl;
    }
    const int closeret = close(sockfd);
    if (closeret) {
        cerr << "Error closing file descriptor " << errno << endl;
    }
}
