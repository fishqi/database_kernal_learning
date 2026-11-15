#include "Page.h"
#include <string.h>

Page::Page(char* buffer) {
    if(NULL != buffer) {
        m_data = buffer;
    }else {
        m_data = NULL;
    }
}

Page::~Page() {
    m_data = NULL;
}

void Page::Init(unsigned int page_id, PAGE_TYPE type) {
    if(NULL == m_data) return;
    if(sizeof(page_header_t) != PAGE_HEADER_SIZE) return;

    memset(m_data, 0, PAGE_SIZE);//初始化内存

    page_header_t page_header;
    page_header.magic = 0;
    page_header.page_id = page_id;
    page_header.page_type = (uint16_t)type;
    page_header.format_version = 0;
    page_header.flags = 1;
    page_header.free_space = PAGE_DATA_SIZE;
    page_header.checksum = 0;
    page_header.reserved = 0;
    memcpy(m_data, &page_header, sizeof(page_header_t));
}

unsigned int Page::GetPageId() const {
    page_header_t page_header;
    memcpy(&page_header, m_data, sizeof(page_header_t));
    return page_header.page_id;
}
   
void Page::SetPageId(unsigned int page_id) {
    page_header_t page_header;
    memcpy(&page_header, m_data, sizeof(page_header_t));
    page_header.page_id = page_id;
    memcpy(m_data, &page_header, sizeof(page_header_t));
}

PAGE_TYPE Page::GetPageType() const {
    page_header_t page_header;
    memcpy(&page_header, m_data, sizeof(page_header_t));
    return (PAGE_TYPE)page_header.page_type;
}
    
void Page::SetPageType(PAGE_TYPE type) {
    page_header_t page_header;
    memcpy(&page_header, m_data, sizeof(page_header_t));
    page_header.page_type = type;
    memcpy(m_data, &page_header, sizeof(page_header_t));
}

char* Page::GetData() {
    return m_data + PAGE_HEADER_SIZE;
}

const char* Page::GetData() const {
    return m_data + PAGE_HEADER_SIZE;
}

unsigned int Page::GetDataSize() const {
    return PAGE_DATA_SIZE;
}

void Page::Reset() {
    if(NULL == m_data) return;
    memset(m_data, 0, PAGE_DATA_SIZE);
}

bool Page::IsValid() const {
    page_header_t page_header;
    memcpy(&page_header, m_data, sizeof(page_header_t));
    
    if (page_header.page_type == PAGE_TYPE_INVALID)
    {
        return false;
    }

    if (page_header.page_type > PAGE_TYPE_INDEX)
    {
        return false;
    }

    return true;
}

int Page::WriteBytes(const void *data, int offset, int size) {
    if(NULL==data || size<=0) return -1;
    if(offset>PAGE_SIZE || PAGE_SIZE-offset<size) return -1;
    
    memcpy(m_data+offset, data, size);
    
    return 0;
}

int Page::ReadBytes(void *data, int offset, int size) {
    if(NULL==data || size<=0) return -1;
    if(offset>PAGE_SIZE || PAGE_SIZE-offset<size) return -1;
    
    memcpy(data, m_data+offset, size);
    
    return 0;
}

