#include <iostream>
#include <string>
#include <thread>
#include <tbb/task_group.h>

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
    //tbb::task_group 创建一个任务组，启动多个任务，一个负责下载，一个负责和用户交互。
    tbb::task_group tg;
    tg.run([&]{
        download("hello.zip");
    });
    tg.run([&]{
        interact();
    });
    tg.wait();
    return 0;
}
