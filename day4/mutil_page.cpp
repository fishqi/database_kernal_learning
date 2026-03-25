#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define SLOT_NUM 1024
#define PAGE_SIZE 4096*1024*4
#define PAGE_NUM 1024

typedef struct page_meat_st {
    unsigned int page_num;
    unsigned int free_page_head;
}page_meta_t;

typedef struct slot_st {
    unsigned int offset;
    unsigned int length;
    unsigned int flag; 
}slot_t;

typedef struct page_header_st {
    unsigned int page_id;
    unsigned int free_offset;
    unsigned int slot_num;
    unsigned int file_offset;
    unsigned int next_free_page;
}page_header_t;;

typedef struct page_st {
    page_header_t header;
    slot_t slot[SLOT_NUM];
    char data[PAGE_SIZE-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM];
}page_t;

class Page {
public:
    Page();
    ~Page();
public:
    int InitPage(unsigned int page_id);
    int WriteRecord(const char *data, int len);
    int ReadRecord(unsigned int slot_id, char *data, unsigned int &len);
private:
    page_t m_page;
};

class PageManager {
public:
    PageManager();
    ~PageManager();
    int InitPages(unsigned int num);//init num pages
    int WriteRecord(const char *data, int len); //write record to slot
    int ReadRecord(char *data, int &len, int slot_id);//read record
    int WritePage(unsigned int page_id); //write page to disk
    int ReadPage(unsigned int page_id); //read page
    int AllocatPage();
    int OpenDBFile(const char *file_path);
    int Persistencepage();
};

Page::Page() {
}

Page::~Page() {
}

int Page::InitPage(unsigned int page_id) {
    if(page_id >= PAGE_NUM) return -1;

    m_page.header.page_id = page_id;
    m_page.header.free_offset = PAGE_SIZE;
    m_page.header.slot_num = 0;
    m_page.header.file_offset = 0;
    m_page.header.next_free_page = page_id + 1;
    
    for(int i=0; i<SLOT_NUM; i++) {
        slot_t *slot = &m_page.slot[i];
        slot->offset = 0;
        slot->length = 0;
        slot->flag = 0;
    }

    memset(m_page.data, 0, PAGE_SIZE-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM);
    
    return 0;
}
 
int Page::WriteRecord(const char *data, int len) {
    if(NULL == data || len<=0) return -1;
    unsigned int slot_id = m_page.header.slot_num;
    if(slot_id >= SLOT_NUM) return -1;

    //no enouth space
    if(m_page.header.free_offset-len <= PAGE_SIZE-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM) {
        return -1;
    }
    m_page.header.free_offset -= len;
    m_page.header.slot_num++;
  
    //insert data to buf head  
    unsigned int free_space_offset = m_page.header.free_offset-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM;
    memcpy(m_page.data+free_space_offset, data, len);
    m_page.slot[slot_id].offset = m_page.header.free_offset;
    m_page.slot[slot_id].length = len;
    m_page.slot[slot_id].flag = 1;
    return 0;
}

int Page::ReadRecord(unsigned int slot_id, char *data, unsigned int &len) {
    if(slot_id >= SLOT_NUM) return -1;
    slot_t *slot = &m_page.slot[slot_id];
    unsigned int data_offset = slot->offset-sizeof(page_header_t)-sizeof(slot_t)*SLOT_NUM;
    unsigned int data_len = slot->length;
    memcpy(data, m_page.data+data_offset, data_len);
    len = data_len;
    return 0;
}

int main () {
    Page *page = new Page;
    int code = page->InitPage(0);
    if(0 != code) return -1;
    printf("init page success\n");

    code = page->WriteRecord("1234", 4);
    code = page->WriteRecord("1235", 4);
    code = page->WriteRecord("12368", 5);
    code = page->WriteRecord("1237", 4);
    printf("write code %d\n", code);
    
    char buf[1024];
    unsigned int len = 0;
    memset(buf, 0, 1024);
    code = page->ReadRecord(0, buf, len);
    printf("read buf is %s\n", buf);

    memset(buf, 0, 1024);
    code = page->ReadRecord(2, buf, len);
    printf("read buf is %s\n", buf);

    memset(buf, 0, 1024);
    code = page->ReadRecord(1, buf, len);
    printf("read buf is %s\n", buf);

    memset(buf, 0, 1024);
    code = page->ReadRecord(3, buf, len);
    printf("read buf is %s\n", buf);
    delete page;
}
