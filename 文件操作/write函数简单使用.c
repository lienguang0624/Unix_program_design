#include <unistd.h>
#include <stdlib.h>

int main()
{
    if ((write(1, "Here is some data\n", 18)) != 18)//向标准输出发送缓冲区内的字符串
        write(2, "A write error has occurred on file descriptor 1\n",46);

    exit(0);
}

