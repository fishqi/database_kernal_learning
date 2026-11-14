#ifndef PAGE_H_
#define PAGE_H_

#include <stdio.h>
#include <stdlib.h>

#define PAGE_HEADER_SIZE 32
#define PAGE_SIZE 4096
#define PAGE_DATA_SIZE PAGE_SIZE-PAGE_HEADER_SIZE

enum PAGE_TYPE
{
    PAGE_TYPE_INVALID = 0,
    PAGE_TYPE_DATA = 1,
    PAGE_TYPE_SLOT = 2,
    PAGE_TYPE_META = 3,
    PAGE_TYPE_INDEX = 4
};

typedef struct page_header_st
{
    uint32_t magic; //页面格式标记
    uint32_t page_id; //页号
    uint16_t page_type; //page类型
    uint16_t format_version; //格式版本
    uint32_t flags; //页面状态标记
    uint32_t free_space;
    uint32_t checksum;
    uint32_t reserved;
}page_header_t;

class Page {
public:
    // 构造函数
    explicit Page(char* buffer);
    //析构函数
    ~Page();
    // 初始化 Page
    void Init(unsigned int page_id, PAGE_TYPE type);
    // 获取 Page ID
    unsigned int GetPageId() const;
    // 设置 Page ID
    void SetPageId(unsigned int page_id);
    // 获取 Page 类型
    PAGE_TYPE GetPageType() const;
    // 设置 Page 类型
    void SetPageType(PAGE_TYPE type);
    // 获取 Page 数据区域
    char* GetData();
    const char* GetData() const;
    // 获取 Page 数据区域大小
    unsigned int GetDataSize() const;
    // 清空 Page
    void Reset();
    // 检查 Page 是否有效
    bool IsValid() const;
private:
    char *m_data;
};

#endif
