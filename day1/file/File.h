#ifndef FILE_H_
#define FILE_H_

class File {
public:
    File();
    ~File();
    int Open(const char *filename);
    int Close();
    int IsOpen();
    int GetFd();
private:
    int m_fd;
};

#endif
