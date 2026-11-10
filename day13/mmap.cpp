#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef struct person_st {
    int age;
    int score;
    char name[12];
}person_t;

int main() {
    int fd = open("test.db", O_CREAT|O_RDWR, 0644);
    if(fd <= 0) {
        printf("open file error\n");
        return 0;
    }
    printf("open file success\n");
    int age = 19;
    write(fd, &age, sizeof(int));
    int score = 100;
    write(fd, &score, sizeof(int));
    const char name[12] = "qixianghui";
    write(fd, name, strlen(name));
    fsync(fd);
    char *data_addr = (char *)mmap(NULL, 18, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    person_t *person = (person_t *)data_addr;
    printf("score is %d\n", person->score);
    printf("age is %d\n", person->age);
    printf("name is %s\n", person->name);
    
    person->age = 22;
    msync(data_addr, 18, MS_SYNC);
    munmap(data_addr, 18);
    data_addr = (char *)mmap(NULL, 18, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    person = (person_t *)data_addr;
    printf("age is %d\n", person->age);
    munmap(data_addr, 18);
    close(fd);
}
