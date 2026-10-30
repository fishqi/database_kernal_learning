#include "File.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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
