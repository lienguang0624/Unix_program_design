#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);//声明线程执行函数
pthread_mutex_t work_mutex; /* protects both work_area and time_to_exit 互斥量 */

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit = 0;

int main() {
    int res;//定义函数执行结果检测变量
    pthread_t a_thread;//定义线程变量
    void *thread_result;//定义线程结束指针
    res = pthread_mutex_init(&work_mutex, NULL);//初始互斥量
    if (res != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&a_thread, NULL, thread_function, NULL);//创建新线程
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&work_mutex);
    printf("Input some text. Enter 'end' to finish\n");
    while(!time_to_exit) {//只要退出标志位不置1，就一直在里面循环
        fgets(work_area, WORK_SIZE, stdin);//捕获用户输入放置在work_area
        pthread_mutex_unlock(&work_mutex);//解锁（解的是59与65行的锁）
        while(1) {
            pthread_mutex_lock(&work_mutex);//加锁
            if (work_area[0] != '\0') {//只有第一个字符不是'\0'了，才解锁（解的是69行的锁）
                pthread_mutex_unlock(&work_mutex);
                sleep(1);
            }
            else {
                break;
            }
        }
    }
    pthread_mutex_unlock(&work_mutex);
    printf("\nWaiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined\n");
    pthread_mutex_destroy(&work_mutex);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) {
    sleep(1);
    pthread_mutex_lock(&work_mutex);//尝试对互斥量加锁，如果已经被锁住将会阻塞直到释放为止
    while(strncmp("end", work_area, 3) != 0) {
        printf("You input %d characters\n", strlen(work_area) -1);
        work_area[0] = '\0';//将第一个字符设为'\0'用于标识
        pthread_mutex_unlock(&work_mutex);//解锁（解的是35行的锁）
        sleep(1);
        pthread_mutex_lock(&work_mutex);
        while (work_area[0] == '\0' ) {//反复解锁上锁，等待新的用户输入，一旦输入进来新的字符，第一个字符就不为'\0'，也就跳出循环了
            pthread_mutex_unlock(&work_mutex);
            sleep(1);
            pthread_mutex_lock(&work_mutex);
        }
    }
    time_to_exit = 1;//一旦输入的是end，将退出标志位置1
    work_area[0] = '\0';
    pthread_mutex_unlock(&work_mutex);
    pthread_exit(0);
}
