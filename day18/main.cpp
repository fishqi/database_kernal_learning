#include "Page.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

int main() {
    char buffer[PAGE_DATA_SIZE];
    Page page(buffer);
    page.Init(0, PAGE_TYPE_DATA);
    
    char test_write_data[1024] = "qixianghui 233";
    page.WriteBytes(test_write_data, 0, strlen(test_write_data));
    char test_read_data[1024] = {0};
    page.ReadBytes(test_read_data, 0, strlen(test_write_data));
    printf("test_read data is %s\n", test_read_data);
    
    int fd = open("test.db", O_RDWR|O_CREAT);
    if(fd <= 0) {
        printf("open file error\n");
        return 0;
    }
    printf("open file success\n");
    if(page.WritePage(fd, 0) == -1) {
        printf("write page error\n");
        return 0;
    }
    printf("write page success\n");
    if(page.ReadPage(fd, 0) == -1) {
        printf("read page error\n");
        return 0;
    }
    printf("read page success\n");
    unsigned int page_id = page.GetPageId();
    printf("read page id is %d\n", page_id);
    printf("read data is %s\n", page.GetData());
}
