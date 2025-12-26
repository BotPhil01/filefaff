// TODO implement small buffer partial read partial write
#include <fstream>
#include <types.h>
using namespace std;
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <array>


const u32 BUF_MAX = 1048576;
// enum class status {
//     OK,
//     ERR_BAD_READ,
//     ERR_BAD_WRITE
// };

// void alloc_buffer(char **const buffer) {
//     assert(buffer != NULL);
//     assert(*buffer == NULL);
//     *buffer = malloc(BUF_MAX);
//     assert(*buffer != NULL);
// }
// // sequential read and write for the time being
// const enum status read_stream(file_desc fd_stream, char **const buffer, ssize_t *const read_bytes) {
//     *read_bytes = read(fd_stream, *buffer, BUF_MAX);
//     // bad read
//     if (*read_bytes == -1) {
//         return ERR_BAD_READ;
//     }
//     // add null terminator
//     return OK;
// }

// const enum status write_stream(file_desc fd_stream, char **const buffer, ssize_t *const buffer_size) {
//     const ssize_t written_bytes = write(fd_stream, *buffer, *buffer_size);
//     if (written_bytes == -1) {
//         return ERR_BAD_WRITE;
//     }
//     return OK;
// }

i32 main() {
    ifstream inp_fs("tests/testin.txt");
    ofstream out_fs("tests/testout.txt", ios_base::openmode::_S_out);
    string buffer(BUF_MAX, 0);
    char *initbuf = &buffer[0];
    inp_fs.read(initbuf, BUF_MAX);
    out_fs.write(initbuf, inp_fs.gcount());
    // writes too many bytes
}
