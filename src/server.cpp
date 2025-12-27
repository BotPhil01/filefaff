#include <cerrno>
#include <unistd.h>
#include <iostream>
#include <types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;
const u16 PORT = 42069;
const size_t BUF_SIZE = 1000000;
// socket identified via integer hence can pass it around like nobody's business


const int sockcreatelisten() {
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Error while creating socket " << errno << endl;
        return -1;
    }
    const sockaddr_in address { 
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = INADDR_ANY };
    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address))) {
        cerr << "Error while binding to address " << errno << endl;
        // cerr << "address " << address.<< endl;
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, 1)) {
        cerr << "Error while listening for connections " << errno << endl;
        close(sockfd);
        return -1;
    }
    return sockfd;
}

string readclientmsg(const int clientfd) {
    string outbuffer{};
    string buffer(BUF_SIZE, 0);

    while (true) {
        const ssize_t readret = read(clientfd, &buffer[0], BUF_SIZE);
        if (readret < 0) {
            cerr << "Error reading from socket " << errno << endl;
            string{};
        }
        outbuffer += buffer;
        if (readret < BUF_SIZE || readret == 0) {
            return outbuffer;
        }
    }
}

i32 main() {
    const int sockfd = sockcreatelisten();
    if (sockfd == -1) return sockfd;
    const int clientfd = accept(sockfd, nullptr, nullptr);

    const string clientstr = readclientmsg(clientfd);
    cout << "Message: " << clientstr << endl;
    if (shutdown(sockfd, SHUT_RD)) {
        cerr << "Error shutting down file descriptor " << errno << endl;
    }
    const int closeret = close(sockfd);
    if (closeret) {
        cerr << "Error closing file descriptor " << errno << endl;
    }
}
