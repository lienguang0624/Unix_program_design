
- [什么是管道](#什么是管道)
- [进程管道](#进程管道)
- [将输出送往popen](#将输出送往popen)
  - [传递更多的数据](#传递更多的数据)
  - [popen的实现过程及优缺点](#popen的实现过程及优缺点)
- [pipe调用](#pipe调用)
- [父进程与子进程](#父进程与子进程)
  - [管道关闭后的读操作](#管道关闭后的读操作)
  - [把管道用作标准输入和标准输出](#把管道用作标准输入和标准输出)
- [管道命名：FIFO](#管道命名fifo)
  - [访问FIFO文件](#访问fifo文件)
    - [使用open打开FIFO文件](#使用open打开fifo文件)
    - [不带O_NONBLOCK标志的O_RDONLY和O_WRONLY](#不带o_nonblock标志的o_rdonly和o_wronly)
    - [带O_NONBLOCK标志的O_RDONLY和O_WRONLY](#带o_nonblock标志的o_rdonly和o_wronly)
    - [对FIFO进行读写操作](#对fifo进行读写操作)

# 什么是管道

一个进程连接数据流到另一个进程，使用术语称为管道（pipe）。

通常是把一个进程的输出连接到另一个进程的输入。


# 进程管道

**popen与pclose函数原型：**
```c
#include <stdio.h>

FILE *popen(const char *command, const char *open_mode);
int pclose(FILE *stream_to_close);
```
popen函数允许一个程序将另一个程序作为新进程来启动。

command字符串是要运行的程序和相应的参数。

open_mode必须是“r”或“w”    

如果open_mode是“r”，被调用函数的输出就可以被调用程序使用，例如可以使用fread函数读取File*文件流指针函数的输出。

如果open_mode是“w”，现有程序就可以通过使用fwrite向被调用程序发送数据。需要注意的是，被调用程序通过标准输入流进行读取，然后做出相应操作。

**案例：**
```c
/*
* popen函数允许一个程序将另一个程序作为新进程来启动 
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    FILE *read_fp;
    char buffer[BUFSIZ + 1];
    int chars_read;
    memset(buffer, '\0', sizeof(buffer));//初始化清空缓存区
    read_fp = popen("uname -a", "r");//使用popen调用启动带有-a选项的uname命令
    if (read_fp != NULL) {
        chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);//然后用返回的文件流读取内容并存储在buffer缓冲区
        if (chars_read > 0) {
            printf("Output was:-\n%s\n", buffer);
        }
        pclose(read_fp);//关闭与之关联的文件流
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
}
```

# 将输出送往popen

## 传递更多的数据

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    FILE *read_fp;
    char buffer[BUFSIZ + 1];
    int chars_read;

    memset(buffer, '\0', sizeof(buffer));
    read_fp = popen("ps ax", "r");
    if (read_fp != NULL) {
        chars_read = fread(buffer, sizeof(char), BUFSIZ/10, read_fp);
        while (chars_read > 0) {
            buffer[chars_read - 1] = '\0';
            printf("Reading %d:-\n %s\n", BUFSIZ/10, buffer);
            chars_read = fread(buffer, sizeof(char), BUFSIZ/10, read_fp);
        }
        pclose(read_fp);
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
}
```
在本例程中，程序将会连续从文件流中读取数据，直到没有数据可读为止。

与上面例程不一样的是，本例程通过块方式发送数据，可避免定义一个非常大的缓冲区


## popen的实现过程及优缺点
使用popen调用运行一个程序时，他首先启动shell，然后将command字符串作为为一个参数传递给他。

优点是可以使用shell的扩展，例如通配符，很方便。

缺点是使用popen不仅要启动被请求的程序，还要启动一个shell，这个会占用额外资源。
# pipe调用
底层函数pipe可以不需要启动一个shell就运行请求的命令

**pipe函数原型：**
```c
#include <unistd.h>

int pipe(int file_descriptor[2]);
```
pipe函数的输入参数是由两个整形类型的文件描述符组成的数组的指针。

写入file_descriptor[1]的所有数据都可以从file_descriptor[0]读出来。

数据基于先进先出的原则（FIFO）进行处理。

**案例：**
```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int data_processed;
    int file_pipes[2];
    const char some_data[] = "123";
    char buffer[BUFSIZ + 1];
    pid_t fork_result;

    memset(buffer, '\0', sizeof(buffer));

    if (pipe(file_pipes) == 0) {
        fork_result = fork();//创建新进程
        if (fork_result == -1) {
            fprintf(stderr, "Fork failure");
            exit(EXIT_FAILURE);
        }

// We've made sure the fork worked, so if fork_result equals zero, we're in the child process.

        if (fork_result == 0) {//父进程读取数组中[0]内容
            data_processed = read(file_pipes[0], buffer, BUFSIZ);
            printf("Read %d bytes: %s\n", data_processed, buffer);
            exit(EXIT_SUCCESS);
        }

// Otherwise, we must be the parent process.

        else {//子进程向数组[1]中写入内容
            data_processed = write(file_pipes[1], some_data,
                                   strlen(some_data));
            printf("Wrote %d bytes\n", data_processed);
        }
    }
    exit(EXIT_SUCCESS);
}
```

# 父进程与子进程

## 管道关闭后的读操作

## 把管道用作标准输入和标准输出

# 管道命名：FIFO

## 访问FIFO文件

### 使用open打开FIFO文件

### 不带O_NONBLOCK标志的O_RDONLY和O_WRONLY

### 带O_NONBLOCK标志的O_RDONLY和O_WRONLY

### 对FIFO进行读写操作

