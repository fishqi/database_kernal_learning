#ifndef SLOT_PAGE_H_
#define SLOT_PAGE_H_

#include "Page.h"

#define SLOT_PAGE_HEADER_SIZE 12

typedef struct slot_page_header_st {
    uint32_t slot_count;
    uint32_t upper;
    uint32_t lower;
}slot_page_header_t;

typedef struct slot_st{
    uint32_t offset;
    uint32_t length;
}slot_t;

class SlotPage {
public:
    SlotPage();
    ~SlotPage();
    int Init(Page *page);
    int InsertData(const char *key, int key_size, const char *value, int value_size);
    int ReadData(unsigned int slot_id, char *key, int &key_size, char *value, int &value_size);
    int GetSlot(int slot_id, slot_t* slot);
private:
    int SerializeData(const char *key, int key_size, const char *value, int value_size, char *data);
    int DeSerializeData(char *key, int &key_size, char *value, int &value_size, char *data);
private:    
    Page *m_page;
};
#endif
