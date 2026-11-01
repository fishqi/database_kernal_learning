#ifndef FILE_H_
#define FILE_H_

#include <unistd.h>
#include <errno.h>

class File {
public:
    File();
    ~File();
    int Open(const char *filename);
    int Close();
    int IsOpen();
    int GetFd();
    int Read(char *data, size_t size);
    int Write(const char *data, size_t size);
private:
    int m_fd;
};

#endif
