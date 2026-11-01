#include "File.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

File::File() {
    m_fd = -1;
}

File::~File() {
    Close();
}

int File::Open(const char *filename) {
    if(NULL == filename) return -1;
    m_fd = open(filename, O_CREAT|O_RDWR, 0644);
    if(m_fd < 0) return -1;
    return 0;
}

int File::Close() {
    if(m_fd < 0) return -1;
    close(m_fd);
    return 0;
}

int File::IsOpen() {
    return m_fd >= 0;
}

int File::GetFd() {
    return m_fd;
}

int File::Read(char *data, size_t size) {
    size_t total = 0;
    while(total < size) {
        size_t n = read(m_fd, data+total, size-total);
        if(n > 0) {
            total += n;
        }else if(0 == n) {
            break;//文件提前结束
        }else {
            if(errno == EINTR) {
                continue;
            }
            return -1;
        }
    }
    return total;
}

int File::Write(const char *data, size_t size) {
    size_t total = 0;
    while(total < size) {
        size_t n = write(m_fd, data+total, size-total);
        if(n > 0) {
            total += n;
        }else if(0 == n) {
            return -1;
        }else {
            if(errno == EINTR) {
                continue;
            }
            return -1;
        }
    }
    return total;
}
