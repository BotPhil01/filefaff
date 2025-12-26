// TODO implement small buffer partial read partial write
#include <fstream>
#include <types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <string>
#include <iostream>

using namespace std;

const u32 BUF_MAX = 1024;

i32 main() {
    cout << "starting client" << endl;
    ifstream inp_fs("tests/testin.txt");
    ofstream out_fs("tests/testout.txt", ios_base::openmode::_S_out);
    string buffer(BUF_MAX, 0);
    char *initbuf = &buffer[0];
    while (inp_fs.good()) {
        inp_fs.read(initbuf, BUF_MAX);
        out_fs.write(initbuf, inp_fs.gcount());
    }
    // want to now read from a socket
}
