#ifndef PAGE_H_
#define PAGE_H_

#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096
#define PAGE_HEADER_SIZE 32

typedef struct page_header_st {
    uint32_t magic; //页面格式标记
    uint32_t page_id; //页号
    uint16_t page_type; //page类型
    uint16_t format_version; //格式版本
    uint32_t flags; //页面状态标记
    uint32_t free_space;
    uint32_t checksum;
    uint32_t reserved[2];
}page_header_t;

typedef struct page_st {
    page_header_t header;
    char data[PAGE_SIZE-PAGE_HEADER_SIZE];
}page_t;

class Page {
public:
    Page();
    ~Page();
    void Initialize(uint32_t page_id);
    uint32_t GetPageId() const;
    uint16_t GetPageType() const;
    page_header* GetPageHeader() const;
    char* GetDataArea();
    const char* GetDataArea() const;
    page_t* GetPageData();
    const page_t* GetPageData()const;
private:
    page_t m_page;
};

#endif
