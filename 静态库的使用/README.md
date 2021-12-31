
# 静态库的建立：
(缺点：同时运行许多应用程序并都使用来自同一函数库的函数时，内存会含有同一函数的多份副本，可以试用共享库解决此问题)

1.	建立源文件 test.c
```c
#include <stdio.h>
Void test_fun(){	
     Printf(“I am here”);
}
```
2.	编译生成test.o，-c的作用是阻止编译器创建一个完整的程序（没有main函数）
```c
$gcc -c test.c
```
3.	编写静态库的头文件Lib.h
```c
Void test_fun();
```
4.	编写调用test.c中test_fun（）函数的程序program.c
```c
#include <stdio.h>
#include <lib.h>
Int main(){
    Test_fun();
}
```
5.	编译program.c，为编译器显示指定目标文件，然后要求编译器编译你的文件并将其与先前编译好的目标模块test.o链接
```c
$gcc -c program.c
$gcc -o program program.o test.o
```
6.	创建静态库文件
```c
Ar crv test_lib.a test.o
```
7.	验证
```c
Gcc -o program program.o test_lib.a
./program
```