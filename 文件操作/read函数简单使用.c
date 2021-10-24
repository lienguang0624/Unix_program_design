#include <unistd.h>
#include <stdlib.h>

int main()
{
    char buffer[128];
    int nread;

    nread = read(0, buffer, 128);//从标准输入读取用户输入放置在buffer缓冲区
    if (nread == -1)
        write(2, "A read error has occurred\n", 26);

    if ((write(1,buffer,nread)) != nread)//将缓冲区内的内容打印至标准输出
        write(2, "A write error has occurred\n",27);

    exit(0);
}
