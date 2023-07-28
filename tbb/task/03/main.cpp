#include <iostream>
#include <string>
#include <thread>
#include <tbb/parallel_invoke.h>

void download(std::string file){
    for(int i=0; i < 10;i++){
        std::cout <<"Downloading " << file
                << " (" << i *10 << "%)..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    std::cout <<"Download complete: "<< file<< std::endl;
}


void interact(){
    std::string name;
    std::cin >> name;
    std::cout <<"Hi, " <<name << std::endl;
}

int main(){
    std::cout<<"threads:" << std::this_thread::get_id() << std::endl;
    //tbb::task_group 创建一个任务组，启动多个任务，一个负责下载，一个负责和用户交互。
    tbb::parallel_invoke([&]{
        download("hello.zip");
    },
    [&]{
        interact();
    },[&]{
        for(auto i : {1,2,3,4,5}){
            std::cout << std::this_thread::get_id() << std::endl;
        }
    });
    return 0;
}
