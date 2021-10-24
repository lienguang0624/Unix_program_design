- [1. Linux文件结构](#1-linux文件结构)
  - [1.1 文件与设备](#11-文件与设备)
- [2. 系统调用和设备驱动程序](#2-系统调用和设备驱动程序)
  - [2.1 Open：打开文件或设备](#21-open打开文件或设备)
  - [2.2 Read：从打开的文件或设备中读数据](#22-read从打开的文件或设备中读数据)
  - [2.3 Write：写数据](#23-write写数据)
  - [2.4 Close：关闭文件或设备](#24-close关闭文件或设备)
  - [2.5 Ioctl：把控制信息传递给设备驱动程序](#25-ioctl把控制信息传递给设备驱动程序)
- [3. 库函数](#3-库函数)
- [4. 底层文件访问](#4-底层文件访问)
  - [4.1 Write系统调用](#41-write系统调用)
  - [4.2 Read系统调用](#42-read系统调用)
  - [4.3 Open系统调用](#43-open系统调用)
  - [4.4 Close系统调用](#44-close系统调用)
  - [4.5 其他与文件管理有关的系统调用](#45-其他与文件管理有关的系统调用)
    - [4.5.1 设置文件的下一个读写位置 Lseek](#451-设置文件的下一个读写位置-lseek)
- [5. 标准IO库](#5-标准io库)
  - [5.1 fopen函数](#51-fopen函数)
  - [5.2 fread函数](#52-fread函数)
  - [5.3 fwrite函数](#53-fwrite函数)
  - [5.4 fclose函数](#54-fclose函数)
  - [5.5 fseek函数](#55-fseek函数)
  - [5.6 fflush函数](#56-fflush函数)
  - [5.7 fgetc，getc，getchar函数](#57-fgetcgetcgetchar函数)
  - [5.8 fgets，gets函数](#58-fgetsgets函数)
- [6. 格式化输入与输出](#6-格式化输入与输出)
  - [6.1 printf函数](#61-printf函数)
  - [6.2 fprintf函数](#62-fprintf函数)
  - [6.3 sprintf函数](#63-sprintf函数)
  - [6.4 scanf函数](#64-scanf函数)
  - [6.5 fscanf函数](#65-fscanf函数)
  - [6.6 sscanf函数](#66-sscanf函数)
  - [6.7 文件流错误](#67-文件流错误)
# 1. Linux文件结构
## 1.1 文件与设备
比较重要的设备文件有三个：
1.	/dev/console
代表系统控制台
2.	/dev/tty
如果一个进程有控制终端的话，那么特殊文件/dev/tty就是这个控制终端
3.	/dev/null
空设备，所有写向这个设备的输出都会被丢弃

# 2. 系统调用和设备驱动程序
## 2.1 Open：打开文件或设备
## 2.2 Read：从打开的文件或设备中读数据
## 2.3 Write：写数据
## 2.4 Close：关闭文件或设备
## 2.5 Ioctl：把控制信息传递给设备驱动程序

# 3. 库函数
直接使用底层调用效率非常低，每次进行底层调用需要从运行用户代码切换到执行内核代码，然后再返回用户代码。而调用库函数则可以在数据满足数据块长度要求时安排执行底层调用从而提高效率。

例如与标准IO库对应的头文件是stdio.h

# 4. 底层文件访问
进程文件描述符
0：标准输入
1：标准输出
2：标准错误
## 4.1 Write系统调用

```c
#include <unistd.h>
Size_t write(int fildes,const void *buf,size_t nbytes);
```
作用是把缓冲区buf的前nbytes个字节写入与文件描述符fildes关联的文件中。
案例：
```c
Write(1,”I am here\n”，10)；
```
## 4.2 Read系统调用

```c
Size_t read(int fildes,void *buf,size_t nbytes);
```
作用是从文件描述符fildes相关联的文件中读取nbytes个字节的数据并存放在缓冲区buf中。
函数返回值为0说明未读入任何数据，返回为-1说明调用出现问题。

案例：
```c
Char buffer[128];
Read(0,buffer,128);
```
## 4.3 Open系统调用
为了创建一个新的文件描述符，需要使用系统调用open

```c
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

Int open(const *path,int oflags);
Int open(const *path,int oflags,mode_t mode);
```
准备打开的文件或设备的名称作为参数path传递给函数，oflags参数用于指定打开文件的方式。

O_RDONLY：以只读方式打开。

O_WRONLY：以只写方式打开。

O_RDWR：以读写方式打开。如果使用带有O_CREAT参数，那么三个参数mode表示新建文件的权限。

案例：
```c
Open(“newfile”,O_CREAT,S_IRUSE_S_IXOTH);
```
## 4.4 Close系统调用

```c
#include <unistd.h>
Int close(int fildes);
```
## 4.5 其他与文件管理有关的系统调用
### 4.5.1 设置文件的下一个读写位置 Lseek

```c
Off_t lseek（Int fildes,off_t offset,int whence）;
```
Fildes:文件描述符

Offset：设定位置

Whence：参开位置设定。SEEK_SET:绝对位置。


# 5. 标准IO库
标准IO库（stdio）及其头文件stdio.h为底层io系统调用提供了一个通用的接口

## 5.1 fopen函数

```c
#include <stdio.h>
FILE *fopen(const char *filename,const char *mode)；
```
以mode方式打开filename，成功时返回非空的FILE*指针，失败返回NULL值。

Mode：

“r”，“rb”：只读

“w”，“wb”：文件格式化并只写

“a”，“ab”：追加只写

“r+”，“rb+”，“r+b”：更新方式打开，读写

## 5.2 fread函数

```c
Size_t fread(void *ptr,size_t size,size_t nitems,FILE *stream)
```
数据从文件流stream中读到由ptr指向的数据缓存区，size指定每个数据记录的长度，nitems给出要传输的记录个数。

## 5.3 fwrite函数

```c
Size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream)
```
从数据缓冲区ptr中读出数据，写入到文件流stream中

## 5.4 fclose函数

```c
Int fclose(FILE *stream)
```
关闭指定文件流stream

## 5.5 fseek函数

```c
Int fseek(FILE *stream, long int offset, int whence)
```
Offset：设定位置

Whence：参开位置设定。

SEEK_SET:绝对位置。

## 5.6 fflush函数

```c
Int fflush(FILE *stream)
```
将文件流中所有尚未写出的数据立刻写出

## 5.7 fgetc，getc，getchar函数

1. fgetc()
```c
Int fgetc(FILE *stream);
```
从文件流中取出下一个字节并把它最为一个字符返回。到达文件尾或出现错误返回EOF。

2. getc()
```c
Int getc(FILE *stream);
```
同fgetc（）

3. getchar()
```c
Int getchar();
```
从标准输入中读取下一个字符

## 5.8 fgets，gets函数

1. fgets()
```c
Char *fgets(char *s,int n ,FILE *stream);
```
将读到的字符写到s指向的字符串里，直到遇见换行符或传输了n-1个字符，或者到达文件尾。函数运行成功返回一个指向字符串s的指针。到达文件尾会返回空指针。

2. gets()
```c
Char *gets(char *s);
```
从标准输入读取数据并丢弃遇到的换行符。并在接受的字符串的尾部加一个null。

# 6. 格式化输入与输出

## 6.1 printf函数
```c
int printf(const char *format,…);
```

把自己的输出送到标准输出

## 6.2 fprintf函数
```c
int fprintf(FILE *stream, const char *format,…)
```

把自己的输出送到指定的文件流stream中

## 6.3 sprintf函数
```c
int sprintf(char *s, const char *format,…)
```

把自己的输出和一个结尾空字符写到作为参数传递过来的字符串s里

## 6.4 scanf函数
```c
int scanf(const char *format,…);
```

将读入的值保存到相应变量中

## 6.5 fscanf函数
```c
int fscanf(FILE *stream,const char *format,…);
```


## 6.6 sscanf函数
```c
int sscanf(const char *s,const char *format,…);
```


## 6.7 文件流错误
1. ferror()
```c
Int ferror(FILE *stream);
```
测试一个文件流的错误标识，如果该标识被设置就返回一个非零值，否则返回零；

2. feof()

```c
Int feof(FILE *stream)
```
测试一个文件流的文件尾标识，效果同上。

