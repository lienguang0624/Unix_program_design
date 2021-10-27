- [什么是进程](#什么是进程)
- [启动新进程](#启动新进程)
  - [等待一个进程](#等待一个进程)
  - [僵尸进程](#僵尸进程)
- [信号](#信号)
  - [发送信号](#发送信号)

# 什么是进程

一个其中运行着一个或多个线程的地址空间和这些线程所需要的系统资源称为进程

# 启动新进程

可以从一个程序内部启动另一个程序，从而创建一个新进程。

**函数原型：**
```c
#include<stdlib.h>
int system (const char *string);
```
system()函数的作用是，运行以字符串的形式传递给他的命令并等待该命令的完成

**案例：**
```c
#include <stdlib.h>
#include <stdio.h>

int main()
{
    printf("Running ps with system\n");
    system("ps ax");
    printf("Done.\n");
    exit(0);
}
```

也可以通过fork（）函数创建一个新进程，这个系统调用复制当前进程，在进程表中创建一个新的进程

**函数原型：**
```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```
父进程中fork调用返回的是子进程的PID。

子进程中fork调用返回的是0。

可通过这个判断当前进程是父进程还是子进程。

**案例：**
```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid;
    char *message;
    int n;

    printf("fork program starting\n");
    pid = fork();//创建子进程
    switch(pid) 
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        message = "This is the child";
        n = 5;
        break;
    default:
        message = "This is the parent";
        n = 3;
        break;
    }

    for(; n > 0; n--) {
        puts(message);
        sleep(1);
    }
    exit(0);
}

```

## 等待一个进程
当使用fork启动一个子进程时，子进程就有了自己的生命周期并独立运行。

可以通过在父进程中调用wait函数让父进程等待子进程的结束。

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *stat_loc)
```
wait系统调用将暂停父进程直到它的子进程结束。该函数返回子进程的PID。


**案例：**
```c
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid;
    char *message;
    int n;
    int exit_code;

    printf("fork program starting\n");
    pid = fork();
    switch(pid) 
    {
    case -1:
        exit(1);
    case 0:
        message = "This is the child";
        n = 5;
        exit_code = 37;
        break;
    default:
        message = "This is the parent";
        n = 3;
        exit_code = 0;
        break;
    }

    for(; n > 0; n--) {
        puts(message);
        sleep(1);
    }

/*  This section of the program waits for the child process to finish.  */

    if(pid) {
        int stat_val;
        pid_t child_pid;

        child_pid = wait(&stat_val);

        printf("Child has finished: PID = %d\n", child_pid);
        if(WIFEXITED(stat_val))
            printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
        else
            printf("Child terminated abnormally\n");
    }
    exit (exit_code);
}

```
## 僵尸进程
    使用fork创建的子进程在终止时，依然存在与父进程之间的关联。

    直到父进程也正常终止或者父进程调用wait才会结束这种关联。

    如果遗忘手动终止子进程，子进程在自然结束后将成为僵尸进程。

    僵尸进程的存在将会一直消耗系统的资源。

# 信号
    信号是系统响应某些条件而产生的一个事件。

    接收到该信号的进程会相应地采取一些行动。

    信号的名称在头文件signal.h中定义。

**信号触发函数原型：**
```c
#include <signal.h>
void (signal(int sig, void (*func)(int)))(int);
```
准备捕获或忽略的信号由参数sig给出。

接收到信号要调用的函数由参数func给出。

**案例：**
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ouch(int sig)
{
    printf("OUCH! - I got signal %d\n", sig);
    (void) signal(SIGINT, SIG_DFL);
}


int main()
{
    (void) signal(SIGINT, ouch);//接收到CTRL+C中断触发后，执行ouch中断函数

    while(1) {
        printf("Hello World!\n");
        sleep(1);
    }
}
```

## 发送信号
进程可以通过调用kill函数向包括他本身在内的其他进程发送一个信号。

**函数原型：**
```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid,int sig);
```
kill函数把参数sig给定的信号发送给由参数PID给出的进程号所指定的进程，成功时返回0


**案例：**
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int alarm_fired = 0;

void ding(int sig)
{
    alarm_fired = 1;
}

/*  在主函数中，我们告诉子进程在等待5s后发送一个SIGGALRM信号给他的父进程 */

int main()
{
    pid_t pid;

    printf("alarm application starting\n");

    pid = fork();
    switch(pid) {
    case -1:
      /* Failure */
      perror("fork failed");
      exit(1);
    case 0:
      /* child */
        sleep(5);
        kill(getppid(), SIGALRM);
        exit(0);
    }

/*  父进程通过一个signal调用安排好捕获SIGALRM信号的工作，然后等待他的到来 */

    printf("waiting for alarm to go off\n");
    (void) signal(SIGALRM, ding);

    pause();//把程序挂起，直到有信号传入
    if (alarm_fired)
        printf("Ding!\n");

    printf("done\n");
    exit(0);
}

```



