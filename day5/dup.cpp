#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("test.data", O_RDWR);
    if(fd < 0) {
        printf("open file error\n");
        return -1;
    }

    int fd2 = dup(fd);
    if(fd2 < 0) {
        printf("dup file error\n");
        return -1;
    }

    char buf[1024] = {0};
    int count = read(fd, buf, 3);
    printf("read buf is %s and count is %d\n", buf, count);

    count = read(fd2, buf, 3);
    printf("read buf is %s and count is %d\n", buf, count);
}
