#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *thread_function(void *arg);//定义线程调用的函数原型

char message[] = "Hello World";

int main() {
    int res;//创建线程函数的返回值，用于检测线程是否创建成功
    pthread_t a_thread;//pthread_create（）的第一个参数，用于引用新线程
    void *thread_result;
    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);//创建新线程
    if (res != 0) {//如果线程创建失败报错
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);//等待线程结束，第一个是等待其结束的线程的标识符，第二个是指向线程返回值的指针（本例程中为pthread_exit（）的返回参数指针）
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined, it returned %s\n", (char *)thread_result);
    printf("Message is now %s\n", message);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    printf("thread_function is running.\n Argument was %s\n", (char *)arg);
    sleep(3);
    strcpy(message, "Bye!");
    pthread_exit("Thank you for the CPU time\n");
}
