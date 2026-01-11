// TODO implement small buffer partial read partial write
#include "socket.h"
#include <writer.h>
#include <types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const u16 PORT = 42069;

i32 main() {
    ff::clientSocket s(PORT);
    s.connect();
    sleep(5);
    s.write("hello world!");
}
