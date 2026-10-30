#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main () {
    int fd = open("fish.db", O_CREAT|O_RDWR, 0644);
    if(-1 == fd) {
        printf("open file error\n");
        return -1;
    }
    printf("open file  %d success\n", fd);
    close(fd);
    return 0;
}
