#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int fd_write = open("test.data", O_CREAT|O_RDWR, 0644);
    int write_a = 5;
    write(fd_write, &write_a, 4);
    
    int write_b = 51;
    write(fd_write, &write_b, 4);
    
    char write_buf[] = "qixianghu i";
    write(fd_write, &write_buf, strlen(write_buf));
    
    int write_c = 52;
    write(fd_write, &write_c, 4);
    
    close(fd_write);

    int fd_read = open("test.data", O_CREAT|O_RDWR, 0644);
    int read_a = 0;
    read(fd_read, &read_a, 4);
    printf("read a is %d\n", read_a);
    
    int read_b = 0;
    read(fd_read, &read_b, 4);
    printf("read b is %d\n", read_b);
    
    char read_buf[11];
    memset(read_buf, 0, 11);
    read(fd_read, read_buf, 11);
    printf("read buf is %s\n", read_buf);
    
    int read_c = 0;
    read(fd_read, &read_c, 4);
    printf("read c is %d\n", read_c);
    
    close(fd_read);
}
