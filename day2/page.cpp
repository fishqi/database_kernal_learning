#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define PAGE_SIZE 4096

typedef struct page_header_st {
    unsigned int magic;
    unsigned int page_id;
    unsigned int free_off;
}page_header_t;

typedef struct page_st {
    page_header_t header;
    char data[PAGE_SIZE-sizeof(page_header_t)];
}page_t;

int page_init(const char *path);
int page_write(int fd, unsigned int page_id, page_t page);
int page_read(int fd, unsigned int page_id, page_t page);


int page_init(const char *path) {
    int fd = open(path, O_CREAT|O_RDWR);
    return fd;
}

int page_write(int fd, unsigned int page_id, page_t page) {
    if(fd<=0) return -1;
    
    int off = page_id * PAGE_SIZE;
    lseek(fd, off, SEEK_SET);
    
    //write page
    write(fd, &page.header.magic, sizeof(unsigned int));
    write(fd, &page.header.page_id, sizeof(unsigned int));
    write(fd, &page.header.free_off, sizeof(unsigned int));
    write(fd, &page.data, PAGE_SIZE-sizeof(page_header_t));
    return 0;
}

int page_read(int fd, unsigned int page_id, page_t *page) {
    if(fd<=0) return -1;
    
    int off = page_id * PAGE_SIZE;
    lseek(fd, off, SEEK_SET);
    
    //read page
    read(fd, &page->header.magic, sizeof(unsigned int));
    read(fd, &page->header.page_id, sizeof(unsigned int));
    read(fd, &page->header.free_off, sizeof(unsigned int));
    read(fd, &page->data, PAGE_SIZE-sizeof(page_header_t));
    return 0;
}

int main() {
    page_t page_w1;
    page_w1.header.magic = 1;
    page_w1.header.page_id = 0;
    memset(page_w1.data, 0, PAGE_SIZE-sizeof(page_header_t));
    strcpy(page_w1.data, "qixianghui_1_test");
    page_w1.header.free_off = sizeof(page_header_t)+strlen(page_w1.data);

    page_t page_w2;
    page_w2.header.magic = 1;
    page_w2.header.page_id = 0;
    memset(page_w2.data, 0, PAGE_SIZE-sizeof(page_header_t));
    strcpy(page_w2.data, "qixianghui_2_test");
    page_w2.header.free_off = sizeof(page_header_t)+strlen(page_w2.data);
    
    int fd = page_init("test.data");
    
    page_write(fd, 0, page_w1);
    page_write(fd, 1, page_w2);
    
    page_t page_r1;
    page_read(fd, 0, &page_r1);
    page_t page_r2;
    page_read(fd, 1, &page_r2);
    close(fd);
    printf("page1 data: %s\n", page_r1.data);
    printf("page2 data: %s\n", page_r2.data);
}
