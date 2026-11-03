#include "FileManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cerrno>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

FileManager::FileManager() {
    m_fd = -1;
}

FileManager::~FileManager() {
    Close();
}

int FileManager::CreateFile(const char *filename) {
    if(NULL == filename || strlen(filename) <= 0) return -1;
    int fd = open(filename, O_CREAT, 0644);
    if(fd <= 0) return -1;
    close(fd);
    return 0;
}

int FileManager::OpenFile(const char *filename) {
    if(NULL == filename || strlen(filename) <= 0) return -1;
    m_fd = open(filename, O_RDWR);
    if(m_fd <= 0) return -1;
    return 0;
}

int FileManager::Read(char *buf, const int size) {
    if(m_fd <= 0) return -1;
    if(NULL == buf || strlen(buf) <= 0 || size <= 0) return -1;
    size_t total = 0;
    while(total < size) {
        size_t count = read(m_fd, buf, size);
        if(-1 == count) {
            if (errno == EINTR)
            {
                continue;
            }

            perror("read");
            return -1;
        }else if(0 == count) {
            break;
        }else {
            total += count;
        }
    }
    return total;
}

int FileManager::Write(const char *buf, const int size) {
    if(m_fd <= 0) return -1;
    if(NULL == buf || strlen(buf) <= 0 || size <= 0) return -1;
    size_t total = 0;
    while(total < size) {
        size_t count = write(m_fd, buf, size);
        if(-1 == count) {
            if (errno == EINTR)
            {
                continue;
            }
            perror("write");
            return -1;
        }else if(0 == count) {
            return -1;
        }else {
            total += count;
        }
    }
    return total;
}

int FileManager::ReadAt(char *buf, const int size, const int offset) {
    if(m_fd <= 0) return -1;
    if(NULL == buf || strlen(buf) <= 0 || size <= 0) return -1;
    size_t total = 0;
    while(total < size) {
        size_t count = pread(m_fd, buf, size, offset+total);
        if(-1 == count) {
            if (errno == EINTR)
            {
                continue;
            }

            perror("read");
            return -1;
        }else if(0 == count) {
            break;
        }else {
            total += count;
        }
    }
    return total;

}

int FileManager::WriteAt(const char *buf, const int size, const int offset) {
    if(m_fd <= 0) return -1;
    if(NULL == buf || strlen(buf) <= 0 || size <= 0) return -1;
    size_t total = 0;
    while(total < size) {
        size_t count = pwrite(m_fd, buf, size, offset+total);
        if(-1 == count) {
            if (errno == EINTR)
            {
                continue;
            }
            perror("write");
            return -1;
        }else if(0 == count) {
            return -1;
        }else {
            total += count;
        }
    }
    return total;
}

int FileManager::GetFileSize() {
    if(m_fd <= 0) return -1;
    struct stat st;
    if(-1 == fstat(m_fd, &st)) return -1;
    return st.st_size;
}

int FileManager::Sync() {
    if(m_fd <= 0) return -1;
    if(-1 == fsync(m_fd)) return -1;
    return 0;
}

int FileManager::Close() {
    if(m_fd <= 0) {
        return -1;
    }
    close(m_fd);
    m_fd = -1;
    return 0;
}

