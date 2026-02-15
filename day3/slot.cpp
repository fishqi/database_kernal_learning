#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define SLOT_NUM 1024
#define PAGE_SIZE 4096*1024

typedef struct slot_st {
    unsigned int offset; //this record's offset in page
    unsigned int length;
    unsigned int flag;//normal:1 delete:0
}slot_t;

typedef struct page_header_st {
    unsigned int page_id;
    unsigned int free_offset;
    unsigned int slot_num;
}page_header_t;

typedef struct page_st {
    page_header_t header;
    slot_t slot[SLOT_NUM];
    char data[PAGE_SIZE-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM];
}page_t;

int page_init(page_t *page) {
    if(!page) return -1;
    page->header.page_id = 0;
    page->header.free_offset = 0;
    page->header.slot_num = 0;
    
    for(int i=0; i<SLOT_NUM; i++) {
        page->slot[i].offset = 0;
        page->slot[i].length = 0;
        page->slot[i].flag = 0;
    }
    
    memset(page->data, 0, PAGE_SIZE-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM);
    return 0;
}

int page_insert_record(page_t *page, const char *data, unsigned int len) {
    if(!page || !data || len<=0) return -1;
    if(len+page->header.free_offset >= PAGE_SIZE-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM) return -1; //no enough space
    
    unsigned int slot_id = page->header.slot_num;
    if(slot_id >= SLOT_NUM) return -1;

    memcpy(page->data+page->header.free_offset, data, len);
    slot_t *slot = &page->slot[slot_id]; 
    slot->offset = page->header.free_offset;
    slot->length = len;
    slot->flag = 1;
    
    page->header.free_offset += len; //update free_offset
    page->header.slot_num++;

    return 0;
}

int page_delete_record(page_t *page, unsigned int slot_id) {
    if(!page) return -1;
    if(slot_id >= SLOT_NUM) return -1;
    page->slot[slot_id].flag = 0;
    return 0;
}

int page_query_record(page_t *page, unsigned int slot_id) {
    if(!page) return -1;
    if(slot_id >= SLOT_NUM) return -1;
    int offset = page->slot[slot_id].offset;
    int length = page->slot[slot_id].length;
    char *buf = (char *)malloc(length);
    memcpy(buf, page->data+offset, length);
    printf("data record offset is :%d, length :%d, data, is: %s\n", offset, length, buf);
    free(buf);
    return 0;
}

int main() {
    page_t page;
    int code = page_init(&page);
    if(0 != code) return 0;
    printf("page init success\n");
    
    code = page_insert_record(&page, "qixianghui123", 13);
    if(0 != code) return 0;
    printf("page insert success\n");
    
    code = page_insert_record(&page, "i xianghui12", 12);
    if(0 != code) return 0;
    printf("page insert success\n");

    code = page_insert_record(&page, "2ixianghui1", 11);
    if(0 != code) return 0;
    printf("page insert success\n");
     
    code = page_query_record(&page, 0);
    if(0 != code) return 0;
    printf("query record success\n");

    code = page_query_record(&page, 2);
    if(0 != code) return 0;
    printf("query record success\n");

    code = page_query_record(&page, 1);
    if(0 != code) return 0;
    printf("query record success\n");
}
