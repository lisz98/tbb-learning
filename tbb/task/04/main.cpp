#include <iostream>
#include <string>
#include <thread>
#include <tbb/parallel_invoke.h>


int main(){
    std::string s = "Hello,world!";
    char ch ='d';
    //并行查找字符串，提高查找效率
    tbb::parallel_invoke([&]{
        for(size_t i =0 ; i < s.size() /2;i++){
            if(s[i] == ch){
                std::cout << "found!" <<std::endl;
            }
        }
    },
    [&]{
        for(size_t i = s.size() /2;i < s.size();i++){
            if(s[i] == ch){
                std::cout << "found!" <<std::endl;
            }
        }
    });
    return 0;
}
