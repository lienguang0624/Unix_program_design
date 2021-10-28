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

