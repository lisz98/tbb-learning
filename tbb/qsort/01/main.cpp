#include <iostream>
#include <cmath>
#include <ticktock.h>
#include <tbb/task_group.h>
#include <tbb/parallel_invoke.h>

//斐波那契数列
int fib1(int n){
    if(n <2){
        return n;
    }
    int first , second;
    tbb::task_group tg;
    tg.run([&](){
        first = fib1(n -1);
    });
    tg.run([&](){
        second = fib1(n -2);
    });
    tg.wait();
    return first + second;
}

int fib2(int n){
    if(n <2){
        return n;
    }
    int first ,second;
    tbb::parallel_invoke([&](){
        first = fib2(n -1);
    },[&](){
        second = fib2(n -2);
    });
    return first + second;
}
auto serial_fib (int n){
    if(n < 2){
        return n;
    }
    int first = serial_fib(n-1);
    int second = serial_fib(n-2);
    return first + second;
};

int fib3(int n){
    if(n <29){
    // 任务分得够细以后，转为串行，缓解调度负担，效果显著
        return serial_fib(n);
    }
    int first ,second;
    tbb::parallel_invoke([&](){
        first = fib3(n -1);
    },[&](){
        second = fib3(n -2);
    });
    return first + second;
}
int  main(){
    TICK(fib1);
    std::cout << fib1(39) << std::endl;
    TOCK(fib1);
    TICK(fib2);
    std::cout << fib2(39) << std::endl;
    TOCK(fib2);
    TICK(fib3);
    std::cout << fib3(39) << std::endl;
    TOCK(fib3);  
    return 0;
}
