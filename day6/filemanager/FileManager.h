#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

class FileManager {
public:
    FileManager();
    ~FileManager();
    int CreateFile(const char *filename);
    int OpenFile(const char *filename);
    int Read(char *buf, const int size);
    int Write(const char *buf, const int size);
    int ReadAt(char *buf, const int size, const int offset);
    int WriteAt(const char *buf, const int size, const int offset);
    int GetFileSize();
    int Sync();
    int Close();
private:
    int m_fd;
};
#endif
