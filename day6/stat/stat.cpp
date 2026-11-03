#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    struct stat st;
    int ret = stat("stat.cpp", &st);
    if(0 != ret) {
        printf("stat file error\n");
        return -1;
    }
    size_t file_size = st.st_size;
    printf("file size is %d\n", file_size);
}
