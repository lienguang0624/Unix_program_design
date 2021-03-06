- [内存管理](#内存管理)
  - [内存分配](#内存分配)
  - [释放内存](#释放内存)
- [文件锁定](#文件锁定)
  - [创建锁文件](#创建锁文件)
  - [区域锁定](#区域锁定)
  - [锁定状态下的读写操作](#锁定状态下的读写操作)
  - [死锁](#死锁)
# 内存管理
## 内存分配
内存分配函数
```c
#include <stdlib.h>
void *malloc(size_t size);
```
通过malloc函数来分配指定大小内存空间的指针

使用案例：

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define A_MEGABYTE (1024 * 1024)

int main() {
    char *some_memory;//定义字符串指针变量
    int  megabyte = A_MEGABYTE;//指定内存空间的大小
    int exit_code = EXIT_FAILURE;

    some_memory = (char *)malloc(megabyte);//分配空间（注意需要类型转换）
    if (some_memory != NULL) {
        sprintf(some_memory, "Hello World\n");//将字符串打印在指定空间内
        printf("%s", some_memory);
        exit_code = EXIT_SUCCESS;
    }
    exit(exit_code);
}
```

## 释放内存
释放内存函数
```c
#include <stdlib.h>
void free(void *ptc_to_memory);
```
释放指定指针所指向的内存空间。需要注意的是，所指向的指针必须是由malloc所分配的内存。

案例：
```c
#include <stdlib.h>
#include <stdio.h>


#define ONE_K (1024)

int main()
{
    char *some_memory;
    int exit_code = EXIT_FAILURE;

    some_memory = (char *)malloc(ONE_K);
    if (some_memory != NULL) {
        free(some_memory);//释放后就不能对这块内存进行读写操作了
        printf("Memory allocated and freed again\n");
        exit_code = EXIT_SUCCESS;
    }
    exit(exit_code);
}
```

# 文件锁定
## 创建锁文件
例如打开并创建一个文件的行为（open函数）等原子操作即为创建了一个锁文件。
```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    int file_desc;
    int save_errno;

    file_desc = open("/tmp/LCK.test", O_RDWR | O_CREAT | O_EXCL, 0444);//创建一个锁文件
    if (file_desc == -1) {
        save_errno = errno;
        printf("Open failed with error %d\n", save_errno);
    }
    else {
        printf("Open succeeded\n");
    }
    exit(EXIT_SUCCESS);
}
```
如果重复执行这段程序，第二次执行将会报错

## 区域锁定
一直全锁着耽误事，可以锁部分，称为**文件段锁定**或**文件区域锁定**

fcntl函数：
```c
#include <fcntl.h>
int fcntl(int fildes,int commend, struct flook *flook_structure);
```
**形参1：int fildes**

    fcntl对一个打开的文件描述符进行操作。

**形参2：int commend**

    根据commend参数的设置完成不同的任务。

    有如下三个用于文件锁定的命令选项：
    1. F_GETLK：用于获取fildes打开的文件的锁信息，获取不到会返回-1
    2. F_SETLK：对fildes指向的文件的某个区域加锁或解锁
    3. F_SETLKW：用于获取fildes打开的文件的锁信息，获取不到会一直等待直到获取成功

**形参3：struct flook （*）flook_structure**

    该结构体包含以下成员：
    1. short l_type

        l_type可有以下三种取值:
        1. F_RDLCK：共享锁（不同进程可同时拥有文件同一区域的共享锁），必须以读或读/写方式打开文件
        2. F_UNLCK：解锁，用于清楚锁
        3. F_WRLCK：独占锁（不同进程只能有一个拥有文件同一区域的独占锁），必须以写或读/写方式打开文件

    4. short l_whence

        l_whence定义了文件的相对字符位置，取值只能为SEEK_SET，SEEK_CUR，SEEK_END其中一个

    5. off_t l_start

        l_start是该区域的第一个字节

    6. off_t l_len

        定义该区域的字节数

    7. off_t l_pid

        记录持有锁的进程

## 锁定状态下的读写操作

需要注意的是，锁定状态下的读写操作以一定要用底层的read和write函数，不能使用IO封装好的fread和fwrite函数。

案例（对内存上锁使用案例.c）：

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

const char *test_file = "/tmp/test_lock";

int main() {
    int file_desc;
    int byte_count;
    char *byte_to_write = "A";
    struct flock region_1;
    struct flock region_2;
    int res;

        /* open a file descriptor 打开一个文件描述符*/
    file_desc = open(test_file, O_RDWR | O_CREAT, 0666);
    if (!file_desc) {
        fprintf(stderr, "Unable to open %s for read/write\n", test_file);
        exit(EXIT_FAILURE);
    }

        /* put some data in the file 给文件添加一些数据*/
    for(byte_count = 0; byte_count < 100; byte_count++) {
        (void)write(file_desc, byte_to_write, 1);
    }

        /* setup region 1, a shared lock, from bytes 10 -> 30  */
        /* 把文件的10-30字节设为区域1，并在其上设置共享锁 */
    region_1.l_type = F_RDLCK;
    region_1.l_whence = SEEK_SET;
    region_1.l_start = 10;
    region_1.l_len = 20; 
    
        /* setup region 2, an exclusive lock, from bytes 40 -> 50 */
        /* 把文件的40-50字节设为区域2，并在其上设置独占锁 */
    region_2.l_type = F_WRLCK;
    region_2.l_whence = SEEK_SET;
    region_2.l_start = 40;
    region_2.l_len = 10;

        /* now lock the file 现在锁定文件*/
    printf("Process %d locking file\n", getpid());
    res = fcntl(file_desc, F_SETLK, &region_1);
    if (res == -1) fprintf(stderr, "Failed to lock region 1\n");
    res = fcntl(file_desc, F_SETLK, &region_2);
    if (res == -1) fprintf(stderr, "Failed to lock region 2\n");    

        /* and wait for a while 然后等一会 */
    sleep(60);

    printf("Process %d closing file\n", getpid());    
    close(file_desc);
    exit(EXIT_SUCCESS);
}

```
对上述上锁程序通过如下程序(测试上锁情况所用案例.c)进行测试，观察上锁情况（先运行上述程序，再运行下面的程序）
```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


const char *test_file = "/tmp/test_lock";
#define SIZE_TO_TRY 5

void show_lock_info(struct flock *to_show);


int main() {
    int file_desc;
    int res;
    struct flock region_to_test;
    int start_byte;
    
        /* open a file descriptor 打开一个文件描述符 */
    file_desc = open(test_file, O_RDWR | O_CREAT, 0666);
    if (!file_desc) {
        fprintf(stderr, "Unable to open %s for read/write", test_file);
        exit(EXIT_FAILURE);
    }

    for (start_byte = 0; start_byte < 99; start_byte += SIZE_TO_TRY) {
            /* set up the region we wish to test 设置希望测试的文件区域*/
        region_to_test.l_type = F_WRLCK;
        region_to_test.l_whence = SEEK_SET;
        region_to_test.l_start = start_byte;
        region_to_test.l_len = SIZE_TO_TRY;
        region_to_test.l_pid = -1;     

        printf("Testing F_WRLCK on region from %d to %d\n", 
               start_byte, start_byte + SIZE_TO_TRY);
        
            /* now test the lock on the file 测试文件上的锁*/
        res = fcntl(file_desc, F_GETLK, &region_to_test);
        if (res == -1) {
            fprintf(stderr, "F_GETLK failed\n");
            exit(EXIT_FAILURE);
        }
        if (region_to_test.l_pid != -1) {
            printf("Lock would fail. F_GETLK returned:\n");
            show_lock_info(&region_to_test);
        }
        else {
            printf("F_WRLCK - Lock would succeed\n");
        }

            /* now repeat the test with a shared (read) lock 用共享锁再测一次 */
            /*（共享锁测试可以通过，独占锁测试将无法通过）*/
            /* set up the region we wish to test 再次设置测试参数*/
        region_to_test.l_type = F_RDLCK;
        region_to_test.l_whence = SEEK_SET;
        region_to_test.l_start = start_byte;
        region_to_test.l_len = SIZE_TO_TRY;
        region_to_test.l_pid = -1;     

        printf("Testing F_RDLCK on region from %d to %d\n", 
               start_byte, start_byte + SIZE_TO_TRY);
        
            /* now test the lock on the file */
        res = fcntl(file_desc, F_GETLK, &region_to_test);
        if (res == -1) {
            fprintf(stderr, "F_GETLK failed\n");
            exit(EXIT_FAILURE);
        }
        if (region_to_test.l_pid != -1) {
            printf("Lock would fail. F_GETLK returned:\n");
            show_lock_info(&region_to_test);            
        }
        else {
            printf("F_RDLCK - Lock would succeed\n");
        }

    } /* for */
    
    close(file_desc);
    exit(EXIT_SUCCESS);
}

void show_lock_info(struct flock *to_show) {
    printf("\tl_type %d, ", to_show->l_type);
    printf("l_whence %d, ", to_show->l_whence);
    printf("l_start %d, ", (int)to_show->l_start);        
    printf("l_len %d, ", (int)to_show->l_len);
    printf("l_pid %d\n", to_show->l_pid);
}
```

## 死锁

假设两个程序想要更新同一个文件，他们需要同时更新文件中的字节1和字节2，程序A选择首先更新字节2，然后更新字节1，程序B反之。

两个程序同时启动，程序A锁定字节2，程序B锁定字节1。

然后程序A尝试锁定字节1，但由于该字节被程序B锁定，所以程序A将会等待。

同时程序B尝试锁定字节2，但由于该字节被程序A锁定，所以程序B也在等待。

这样两个程序都无法向下执行，该现象称为死锁。










