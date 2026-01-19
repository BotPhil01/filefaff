// TODO implement small buffer partial read partial write
#include "httpCommand.h"
#include "httpSocket.h"
#include <ports.h>
#include <raiiSocket.h>
#include <writer.h>
#include <types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

i32 main(int argc, char** argv) {
    if (argc > 1) {
        ff::http::clientSocket s{PORT};
        ff::http::get m{argv[1], {}, {}};
        s.send(m);
        std::cout << s.receive().string();
    }
}
