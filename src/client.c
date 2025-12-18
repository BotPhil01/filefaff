#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

const u32 BUF_MAX = 1048576;
enum status {
    OK,
    ERR_BAD_READ,
    ERR_BAD_WRITE
};

void alloc_buffer(char **const buffer) {
    assert(buffer != NULL);
    assert(*buffer == NULL);
    *buffer = malloc(BUF_MAX);
    assert(*buffer != NULL);
}
// sequential read and write for the time being
const enum status read_stream(file_desc fd_stream, char **const buffer, ssize_t *const read_bytes) {
    *read_bytes = read(fd_stream, *buffer, BUF_MAX);
    // bad read
    if (*read_bytes == -1) {
        return ERR_BAD_READ;
    }
    // add null terminator
    return OK;
}

const enum status write_stream(file_desc fd_stream, char **const buffer, ssize_t *const buffer_size) {
    const ssize_t written_bytes = write(fd_stream, *buffer, *buffer_size);
    if (written_bytes == -1) {
        return ERR_BAD_WRITE;
    }
    return OK;
}

i32 main() {
    const mode_t mode_prev = umask(0666);
    const file_desc fd_inp = open("tests/testin.txt", O_RDONLY);
    // flags mode
    const file_desc fd_out = open("tests/testout.txt", O_TRUNC | O_CREAT | O_RDWR, 0666);
    // const u32 res = chmod("tests/testout.txt", 0666);
    char *buffer = NULL;
    alloc_buffer(&buffer);
    ssize_t read_bytes = 0;

    enum status tmp_status;
    if (fd_inp == -1 || fd_out == -1) {
        printf("BAD FILE DESCRIPTORS inp:%d out:%d\nerrno:%d\n", fd_inp, fd_out, errno);
        goto cleanup;
    }

    // if (res == -1) {
    //     printf("BAD CHMOD errno%d\n", errno);
    //     goto cleanup;
    // }


    tmp_status = read_stream(fd_inp, &buffer, &read_bytes);
    if (tmp_status == ERR_BAD_READ) {
        printf("BAD READ FROM FILE errno:%d\n", errno);
        goto cleanup;
    }
    tmp_status = write_stream(fd_out, &buffer, &read_bytes);
    if (tmp_status == ERR_BAD_WRITE) {
        printf("BAD WRITE TO FILE fd:%d errno:%d\n", fd_out, errno);
        goto cleanup;
    }
cleanup:
    umask(mode_prev);
    if (fd_inp != -1) close(fd_inp);
    if (fd_out != -1) close(fd_out);
    free(buffer);
}

// input from testin.txt
// output to testout.txt
// need to producer consumer problem
// read from file while able to output to a file
