#include "SlotPage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SlotPage::SlotPage() {
    m_page = NULL;
}

SlotPage::~SlotPage() {
    m_page = NULL;
}

int SlotPage::Init(Page *page) {
    if(NULL == page) return -1;
    m_page = page;

    //填充slot_page_header
    slot_page_header_t header;
    header.slot_count = 0;
    header.lower = PAGE_HEADER_SIZE+SLOT_PAGE_HEADER_SIZE;
    header.upper = PAGE_SIZE;
    memcpy(m_page->GetData()+PAGE_HEADER_SIZE, &header, sizeof(slot_page_header_t));
    
    m_page->SetFreeSpace(PAGE_SIZE-PAGE_HEADER_SIZE-SLOT_PAGE_HEADER_SIZE);
    return 0;
}

int SlotPage::InsertData(const char *key, int key_size, const char *value, int value_size) {
    //参数检查
    if(NULL==key || NULL == value || key_size<=0 || value_size<0) {
        return  -1;
    }

    //读取slot_page_header
    slot_page_header_t header;
    memcpy(&header, m_page->GetData()+PAGE_HEADER_SIZE, sizeof(slot_page_header_t));
 
    //检查剩余空间
    uint16_t record_size = sizeof(int)*2 + key_size + value_size;
    uint16_t require_size = record_size + sizeof(slot_t);
    uint16_t free_space = header.upper-header.lower;
    if(require_size > free_space) return -1;
     
    //序列化数据
    char *data = (char *)malloc(record_size);
    int ret = SerializeData(key, key_size, value, value_size, data);
    if(ret < 0) {
        free(data);
        data = NULL;
        return -1;
    }

    //设置slot
    slot_t slot;
    slot.offset = header.upper-record_size;
    slot.length = record_size;
    
    //先写data数据
    ret = m_page->WriteBytes(data, slot.offset, slot.length);
    free(data);
    data = NULL;
    if(-1 == ret) {
        return -1;
    }

    //写入slot数据
    memcpy(m_page->GetData()+header.lower, &slot, sizeof(slot_t));
    int slot_id = header.slot_count;
    //更新header
    header.slot_count++;
    header.lower += sizeof(slot_t);
    header.upper -= slot.length;
    memcpy(m_page->GetData()+PAGE_HEADER_SIZE, &header, sizeof(slot_page_header_t));

    //更新剩余空间 
    m_page->SetFreeSpace(free_space-require_size);

    return slot_id;//返回slot_id
}

int SlotPage::ReadData(unsigned int slot_id, char *key, int &key_size, char *value, int &value_size) {
    if(NULL==key || NULL==value) return -1;

    slot_t slot;
    int ret = GetSlot(slot_id, &slot);
    if(-1 == ret) return -1;
    char *data = m_page->GetData();
    ret = DeSerializeData(key, key_size, value, value_size, data+slot.offset);
    
    return ret;
}

int SlotPage::FindKey(const char *key, int key_size, char *value, int &value_size) {
    if(NULL==key || key_size<=0) return -1;
    //首先读取slot_count
    slot_page_header_t header;
    memcpy(&header, m_page->GetData()+PAGE_HEADER_SIZE, sizeof(slot_page_header_t));
    int slot_count = header.slot_count;

    //遍历所有的key
    for(int i=0; i<slot_count; i++) {
        slot_t slot;
        int ret = GetSlot(i, &slot);
        if(-1 == ret) return -1;
        char *data = m_page->GetData();
        if(0 == memcmp(data+slot.offset+sizeof(int), key, key_size) ) {
            memcpy(&value_size, data+slot.offset+sizeof(int)+key_size, sizeof(int));
            memcpy(value, data+slot.offset+sizeof(int)*2+key_size, value_size);
            return i;
        }
    }
    return -1;
}

int SlotPage::GetSlot(int slot_id, slot_t* slot) {
    slot_page_header_t header;
    memcpy(&header, m_page->GetData()+PAGE_HEADER_SIZE, sizeof(slot_page_header_t));
    if(slot_id<0 || slot_id>=header.slot_count) return -1;

    off_t slot_offset = slot_id*sizeof(slot_t)+PAGE_HEADER_SIZE + SLOT_PAGE_HEADER_SIZE;
    char *data = m_page->GetData();
    if(NULL==data) return -1;
    memset(slot, 0, sizeof(slot_t));
    memcpy(slot, data+slot_offset, sizeof(slot_t));
    return 0;
}

int SlotPage::SerializeData(const char *key, int key_size, const char *value, int value_size, char *data) {
    if(NULL==key || NULL==value || key_size<=0 || value_size<=0) {
        return -1;
    }
    off_t offset = 0;
    memcpy(data+offset, &key_size, sizeof(int));
    offset += sizeof(int);

    memcpy(data+offset, key, key_size);
    offset += key_size;
    
    memcpy(data+offset, &value_size, sizeof(int));
    offset += sizeof(int);

    memcpy(data+offset, value, value_size);
    return 0;
}

int SlotPage::DeSerializeData(char *key, int &key_size, char *value, int &value_size, char *data) {
    if(NULL==key || NULL==value || data==NULL) {
        return -1;
    }
    off_t offset = 0;
    memcpy(&key_size, data+offset, sizeof(int));
    offset += sizeof(int);

    memcpy(key, data+offset, key_size);
    offset += key_size;
    
    memcpy(&value_size, data+offset, sizeof(int));
    offset += sizeof(int);

    memcpy(value, data+offset, value_size);
    return 0;
}
