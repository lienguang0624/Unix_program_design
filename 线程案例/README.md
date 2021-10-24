- [线程相关函数分析及轮子分享](#线程相关函数分析及轮子分享)
  - [1. 什么是线程](#1-什么是线程)
  - [2. 进程与线程的区别](#2-进程与线程的区别)
  - [3. 线程主要调用函数解析](#3-线程主要调用函数解析)
    - [3.1 创建线程函数原型](#31-创建线程函数原型)
    - [3.2 终止线程函数原型](#32-终止线程函数原型)
    - [3.3 等待线程信息函数原型](#33-等待线程信息函数原型)
  - [4. 同步主要调用函数解析](#4-同步主要调用函数解析)
    - [4.1 信号量](#41-信号量)
      - [4.1.1 信号量创建函数原型](#411-信号量创建函数原型)
      - [4.1.2 增加信号量操作函数原型](#412-增加信号量操作函数原型)
      - [4.1.3 减少信号量操作函数原型](#413-减少信号量操作函数原型)
      - [4.1.4 清理信号量操作函数原型](#414-清理信号量操作函数原型)
    - [4.2 互斥量](#42-互斥量)
      - [4.2.1 互斥量创建函数原型](#421-互斥量创建函数原型)
      - [4.2.2 互斥量加锁函数原型](#422-互斥量加锁函数原型)
      - [4.2.3 互斥量减锁函数原型](#423-互斥量减锁函数原型)
      - [4.2.4 互斥量销毁函数原型](#424-互斥量销毁函数原型)
  - [5.线程属性设定调用函数解析](#5线程属性设定调用函数解析)
    - [5.1 脱离线程](#51-脱离线程)
    - [5.2 线程优先级调度](#52-线程优先级调度)

# 线程相关函数分析及轮子分享
## 1. 什么是线程
    在一个程序中的多个执行路线叫做线程，fork得到的是进程
    创建新线程比创建新进程有更明显的优势，新线程的创建代价要比新进程小得多。
    一般而言，线程之间的切换需要操作系统做的工作要比进程之间的切换少得多。
    但由于线程之间变量共享，因此多线程程序的调试很困难，线程之间的交互难以控制。
## 2. 进程与线程的区别
        进程就是上下文切换之间的程序执行的部分。是运行中的程序的描述，也是对应于该段CPU执行时间的描述。
        进程，与之相关的东东有寻址空间，寄存器组，堆栈空间等。即不同的进程，这些东东都不同，从而能相互区别。
        线程是共享了进程的上下文环境，的更为细小的CPU时间段。线程主要共享的是进程的地址空间。
        进程和线程都是一个时间段的描述，是CPU工作时间段的描述，不过是颗粒大小不同。
        进程主要是环境的搭建与资源的分配，每个进程所能访问的内存都是圈好的。一人一份，谁也不干扰谁。还有内存的分页，虚拟地址我就不深入探讨了。这里给大家想强调的就是，进程需要管理好它的资源。
        线程是进程的一部分，线程主抓中央处理器执行代码的过程，线程关注的是中央处理器的运行，而不是内存等资源的管理。其余的资源的保护和管理由整个进程去完成。

## 3. 线程主要调用函数解析

### 3.1 创建线程函数原型
```c
#include <pthread.h>
Int pthread_create(pthread_t *thread,pthread_attr_t *attr, void *(*start_routinr)(void *),void *arg);
```
作用是创建一个新线程，第一个参数pthread_t *thread在线程被创建时写入一个标识符，将会用此标识符来引用新线程。第二个参数pthread_attr_t *attr用于设置线程的属性，一般设置为NULL。最后两个参数分别告诉线程将要启动执行的函数和要传递给该线程的参数。

### 3.2 终止线程函数原型
```c
Void pthread_exit(void *retval);
```
作用是终止线程并返回一个指向某个对象的指针

### 3.3 等待线程信息函数原型
```c
Int pthread_join(pthread_t th,void **thread_return)
```
作用是收集子进程信息,第一个参数制定了要等待的线程，线程通过pthread_create返回的标识符里指定。第二个参数指向线程的返回值。

## 4. 同步主要调用函数解析
### 4.1 信号量
#### 4.1.1 信号量创建函数原型
```c
#include<semaphore.h>
Int sem_init(sem_t *sem,int pshared, unsigned int value);
```
函数初始化由sem指向的信号量对象，设置他的共享选项，并给予一个初始的整数值。
Pshared参数控制信号量的类型，如果其值为0，表示这个信号量是当前进程的局部信号量。

#### 4.1.2 增加信号量操作函数原型
```c
Int sem_post(sem_t *sem);
```
通过原子操作的形式给信号量的值加一。

#### 4.1.3 减少信号量操作函数原型
```c
Int sem_wait(sem_t *sem);
```
通过原子操作的形式给信号量的值在非零时减一。

#### 4.1.4 清理信号量操作函数原型
```c
Int sem_destory(sem_t *sem);
```

### 4.2 互斥量
#### 4.2.1 互斥量创建函数原型
```c
#include <pthread.h>
Int pyhread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
```
第一个参数Mutex为先前声明过的指针。第二个参数mutexattr为互斥量的属性，默认为fast。（这里有个问题，如果一个线程已经lock，再对其调用lock函数，程序就会阻塞。又因为拥有互斥量的这个线程正是现在被阻塞的线程，所以互斥量就永远不会解锁了，程序就会进入死锁状态。问题可以通过改变互斥量的属性解决，可以让他检查这种情况并返回一个错误）
#### 4.2.2 互斥量加锁函数原型
```c
Int pthread_mutex_lock(pthread_mutex_t *mutex)
```
尝试对互斥量加锁，如果已经被锁住将会阻塞直到释放为止。
#### 4.2.3 互斥量减锁函数原型
```c
Int pthread_mutex_unlock(pthread_mutex_t *mutex)
```
对互斥量解锁。
#### 4.2.4 互斥量销毁函数原型
```c
Int pthread_mutex_destroy(pthread_mutex_t *mutex)
```
销毁互斥量

## 5.线程属性设定调用函数解析
### 5.1 脱离线程
脱离线程即为子线程不会重复使用，使用一次结束后自动销毁
```c
pthread_t a_thread;
res = pthread_attr_init(&thread_attr);
```
初始化线程属性对象
```c
res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
```
**设定其原先的进程不再等待与他创建的子线程重新合并**（不能调用pthread_join来获取另一个线程的退出状态）
```c
res = pthread_create(&a_thread, &thread_attr, thread_function, (void *)message);
```
运行线程并使用线程属性thread_attr
```c
(void)pthread_attr_destroy(&thread_attr);
```
属性用完后进行清理回收

### 5.2 线程优先级调度
```c
res = pthread_attr_init(&thread_attr);//初始化线程属性信息
if (res != 0) {
    perror("Attribute creation failed");
    exit(EXIT_FAILURE);
}
res = pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);//设定调度策略
if (res != 0) {
    perror("Setting schedpolicy failed");
    exit(EXIT_FAILURE);
}
res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);//设定脱离属性
if (res != 0) {
    perror("Setting detached attribute failed");
    exit(EXIT_FAILURE);
}
res = pthread_create(&a_thread, &thread_attr, thread_function, (void *)message);//将设定好属性的进程运行
if (res != 0) {
    perror("Thread creation failed");
    exit(EXIT_FAILURE);
}
max_priority = sched_get_priority_max(SCHED_OTHER);//查找允许的优先级范围
min_priority = sched_get_priority_min(SCHED_OTHER);
scheduling_value.sched_priority = min_priority;//初始化优先级为低
res = pthread_attr_setschedparam(&thread_attr, &scheduling_value);//设定优先级
```
