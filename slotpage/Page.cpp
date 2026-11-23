#include "Page.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

Page::Page(char* buffer) {
    m_data = buffer;
    memset(m_data, 0, PAGE_SIZE);
}

Page::~Page() {
    m_data = NULL;
}

void Page::Init(unsigned int page_id, PAGE_TYPE type) {
    if(NULL == m_data)  {
        printf("data is null\n");
        return;
    }
    if(sizeof(page_header_t) != PAGE_HEADER_SIZE) return;

    memset(m_data, 0, PAGE_SIZE);//初始化内存
    
    page_header_t header;
    header.page_id = page_id;
    header.page_type = (uint16_t)type;
    header.format_version = 0;
    header.flags = 1;
    header.free_space = PAGE_DATA_SIZE;
    
    memcpy(m_data, &header, sizeof(page_header_t));
}

unsigned int Page::GetPageId() const {
    page_header_t header;
    memcpy(&header.page_id, m_data, sizeof(uint32_t));
    return header.page_id;
}
   
void Page::SetPageId(unsigned int page_id) {
    page_header_t header;
    memcpy(&header.page_id, m_data, sizeof(uint32_t));
    header.page_id = page_id;
    memcpy(m_data, &header.page_id, sizeof(uint32_t));
}

PAGE_TYPE Page::GetPageType() const {
    page_header_t header;
    memcpy(&header.page_type, m_data+sizeof(uint32_t), sizeof(uint16_t));
    return (PAGE_TYPE)header.page_type;
}
    
void Page::SetPageType(PAGE_TYPE type) {
    page_header_t header;
    memcpy(&header.page_type, m_data+sizeof(uint32_t), sizeof(uint16_t));
    header.page_type = type;
    memcpy(m_data+sizeof(uint32_t), &header.page_type, sizeof(uint16_t));
}

unsigned int Page::GetFormatVersion() const {
    page_header_t header;
    memcpy(&header.format_version, m_data+sizeof(uint32_t)+sizeof(uint16_t), sizeof(uint16_t));
    return header.format_version;
}
    
void Page::SetFormatVersion(uint16_t format_version) {
    page_header_t header;
    memcpy(&header.format_version, m_data+sizeof(uint32_t)+sizeof(uint16_t), sizeof(uint16_t));
    header.format_version = format_version;
    memcpy(m_data+sizeof(uint32_t)+sizeof(uint16_t), &header.format_version, sizeof(uint16_t));
}

unsigned int Page::GetFlags() const {
    page_header_t header;
    memcpy(&header.flags, m_data+sizeof(uint32_t)+sizeof(uint16_t)*2, sizeof(uint32_t));
    return header.flags;
}

void Page::SetFlags(uint32_t flags) {
    page_header_t header;
    memcpy(&header.flags, m_data+sizeof(uint32_t)+sizeof(uint16_t)*2, sizeof(uint32_t));
    header.flags = flags;
    memcpy(m_data+sizeof(uint32_t)+sizeof(uint16_t)*2, &header.flags, sizeof(uint32_t));
}

unsigned int Page::GetFreeSpace() const {
    page_header_t header;
    memcpy(&header.free_space, m_data+sizeof(uint32_t)*2+sizeof(uint16_t)*2, sizeof(uint32_t));
    return header.free_space;
}

void Page::SetFreeSpace(uint32_t free_space) {
    page_header_t header;
    memcpy(&header.free_space, m_data+sizeof(uint32_t)*2+sizeof(uint16_t)*2, sizeof(uint32_t));
    header.free_space = free_space;
    memcpy(m_data+sizeof(uint32_t)*2+sizeof(uint16_t)*2, &header.free_space, sizeof(uint32_t));
}

char* Page::GetData() {
    return m_data;
}

const char* Page::GetData() const {
    return m_data;
}

unsigned int Page::GetDataSize() const {
    return PAGE_DATA_SIZE;
}

void Page::Reset() {
    if(NULL == m_data) return;
    memset(m_data, 0, PAGE_SIZE);
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
    if(offset>PAGE_SIZE || PAGE_SIZE-offset<size) {
        return -1;
    }
    
    memcpy(m_data+offset, data, size); 
    return 0;
}

int Page::ReadBytes(void *data, int offset, int size) {
    if(NULL==data || size<=0) return -1;
    if(offset>PAGE_SIZE || PAGE_SIZE-offset<size) return -1;
    
    memcpy(data, m_data+offset, size);
    return 0;
}

int Page::SerializePage(char *data) {
    if(NULL == data) return -1;

    memset(data, 0, PAGE_SIZE); 
    memcpy(data, m_data, PAGE_SIZE);
    
    return 0;
}

int Page::DeSerializePage(const char *data) {
    if(NULL == data) return -1;

    memset(m_data, 0, PAGE_DATA_SIZE);

    
    memcpy(m_data, data, PAGE_SIZE);
    return 0;
}

int Page::WritePage(int fd, int page_id) {
    if(NULL == m_data || fd <= 0) return -1;
    
    off_t offset = page_id * PAGE_SIZE;
    char buffer[PAGE_SIZE];
    if(SerializePage(buffer) < 0) return -1;
    
    size_t total_write = 0;
    while(total_write < PAGE_SIZE) {
        size_t n = pwrite(fd, buffer+total_write, PAGE_SIZE-total_write, offset+total_write);
        if(n < 0) {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        if(0 == n) {
            return -1;
        }
        total_write += n;
    }

    return 0;
}

int Page::ReadPage(int fd, int page_id) {
    if(NULL == m_data || fd <= 0) return -1;
    off_t offset = page_id * PAGE_SIZE;
    
    char buffer[PAGE_SIZE];
    size_t total_read = 0;
    while(total_read < PAGE_SIZE) {
        size_t n = pread(fd, buffer+total_read, PAGE_SIZE-total_read, offset+total_read);
        if(n < 0) {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        if(0 == n) {
            return -1;
        }
        total_read += n;
    }

    if(DeSerializePage(buffer) < 0) return -1;
    
    return 0;
}
