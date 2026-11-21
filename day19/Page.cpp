#include "Page.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

Page::Page(char* buffer) {
    if(NULL != buffer) {
        m_data = buffer;
    }else {
        m_data = NULL;
    }
    memset(&m_header, 0, sizeof(page_header_t));
}

Page::~Page() {
    m_data = NULL;
}

void Page::Init(unsigned int page_id, PAGE_TYPE type) {
    if(NULL == m_data) return;
    if(sizeof(page_header_t) != PAGE_HEADER_SIZE) return;

    memset(m_data, 0, PAGE_DATA_SIZE);//初始化内存
    memset(&m_header, 0, sizeof(page_header_t));

    m_header.magic = 0;
    m_header.page_id = page_id;
    m_header.page_type = (uint16_t)type;
    m_header.format_version = 0;
    m_header.flags = 1;
    m_header.free_space = PAGE_DATA_SIZE;
    m_header.checksum = 0;
    m_header.reserved[0] = 0;
    m_header.reserved[1] = 0;
    //memset(m_header.reserved, 0, sizeof(m_header.reserved));
    //memcpy(m_data, &m__header, sizeof(page_header_t));
}

unsigned int Page::GetPageId() const {
    return m_header.page_id;
}
   
void Page::SetPageId(unsigned int page_id) {
    m_header.page_id = page_id;
}

PAGE_TYPE Page::GetPageType() const {
    return (PAGE_TYPE)m_header.page_type;
}
    
void Page::SetPageType(PAGE_TYPE type) {
    m_header.page_type = type;
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
    if(offset>PAGE_DATA_SIZE || PAGE_DATA_SIZE-offset<size) return -1;
    
    memcpy(m_data+offset, data, size); 
    return 0;
}

int Page::ReadBytes(void *data, int offset, int size) {
    if(NULL==data || size<=0) return -1;
    if(offset>PAGE_DATA_SIZE || PAGE_DATA_SIZE-offset<size) return -1;
    
    memcpy(data, m_data+offset, size);
    return 0;
}

int Page::SerializePage(char *data) {
    if(NULL == data) return -1;

    memset(data, 0, PAGE_SIZE);
    
    off_t offset = 0;
    memcpy(data+offset, &m_header.magic, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(data+offset, &m_header.page_id, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(data+offset, &m_header.page_type, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    
    memcpy(data+offset, &m_header.format_version, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    memcpy(data+offset, &m_header.flags, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(data+offset, &m_header.free_space, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(data+offset, &m_header.checksum, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(data+offset, &m_header.reserved[0], sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(data+offset, &m_header.reserved[1], sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(data+offset, m_data, PAGE_DATA_SIZE);
    return 0;
}

int Page::DeSerializePage(const char *data) {
    if(NULL == data) return -1;

    memset(m_data, 0, PAGE_DATA_SIZE);

    off_t offset = 0;
    memcpy(&m_header.magic, data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&m_header.page_id, data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(&m_header.page_type, data+offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    
    memcpy(&m_header.format_version, data+offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    memcpy(&m_header.flags, data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(&m_header.free_space, data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(&m_header.checksum, data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    memcpy(&m_header.reserved[0], data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&m_header.reserved[1], data+offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(m_data, data+PAGE_HEADER_SIZE, PAGE_DATA_SIZE);
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
