// TODO implement small buffer partial read partial write
#include <fstream>
#include <writer.h>
#include <types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

const u32 BUF_MAX = 1024;
const u16 PORT = 42069;

[[nodiscard("Ignoring return value will cause a hanging socket")]] const int sockcreateconnect() {
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Error while creating socket " << errno << endl;
        return -1;
    }
    const sockaddr_in serveraddr = {
        .sin_family = AF_INET, 
        .sin_port = htons(PORT),
        .sin_addr = INADDR_ANY
    };

    const int conret = connect(
            sockfd,
            (struct sockaddr *) &serveraddr,
            sizeof(serveraddr)
            );
    if (conret != 0) {
        cerr << "Error when connecting to server " << errno << endl;
        return -1;
    }
    return sockfd;
}

// send respoonse
//

const int sendcommand() {

    return 0;
}

i32 main() {
    const int sockfd = sockcreateconnect();
    ifstream inp_fs("tests/testin.txt");

    fdwrite(sockfd, inp_fs);
    close(sockfd);

    // client sends to server
    // client has to send a ftp request
    // client responds with a socket
    // want client to make one request for testin.txt for example
}
