#include "Page.h"
#include "SlotPage.h"
#include "string.h"

int main() {
    char buffer[4096];
    Page *page = new Page(buffer);
    page->Init(0, PAGE_TYPE_DATA);
    printf("page init success\n");
    SlotPage *slot_page = new SlotPage();
    int ret = slot_page->Init(page);
    if(-1 == ret) {
        printf("init slot page error\n");
        return -1;
    }
    printf("slot page init success\n");

    int slot_id = slot_page->InsertData("name", 4, "qixianghui", 10);
    if(-1 == ret) {
        printf("insert data error\n");
        return -1;
    }
    printf("insert data %d success\n", slot_id);
   
    slot_id = slot_page->InsertData("age", 3, "19", 2);
    if(-1 == ret) {
        printf("insert data error\n");
        return -1;
    }
    printf("insert data %d success\n", slot_id);
    
    char key[1024] = {0};
    char value[1024] = {0};
    int key_size = 0;
    int value_size = 0;
    ret = slot_page->ReadData(0, key, key_size, value, value_size);
    if(-1 == ret) {
        printf("read data error");
        return 0;
    }
    printf("read key is %s key size is %d value is %s value size is %d\n", key, key_size, value, value_size);

    memset(key, 0, 1024);
    memset(value, 0, 1024);
    key_size = 0;
    value_size = 0;
    ret = slot_page->ReadData(1, key, key_size, value, value_size);
    if(-1 == ret) {
        printf("read data error");
        return 0;
    }
    printf("read key is %s key size is %d value is %s value size is %d\n", key, key_size, value, value_size);
}
