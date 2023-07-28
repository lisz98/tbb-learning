#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <tbb/parallel_for_each.h>
#include <ticktock.h>
#include <tbb/pipeline.h>

struct Data{
    std::vector<float> arr;

    Data(){
        arr.resize(std::rand() % 100 * 500 + 100000);
        for(size_t i =0 ; i < arr.size() ; ++i){
            arr[i] = std::rand() * (1.f / (float)RAND_MAX);
        }
    }

    void step1(){
        for(size_t i =0 ; i < arr.size();i++){
            arr[i] += 3.14f;
        }
    }
    void step2(){
        std::vector<float> tmp(arr.size());
        for(size_t i =0 ; i < arr.size() -1;i++){
            tmp[i] = arr[i - 1] + arr[i] + arr[i +1 ];
        }
        std::swap(tmp,arr);
    }
    void step3(){
        for(size_t i =0 ; i < arr.size();i++){
            arr[i] = std::sqrt(std::abs(arr[i]));
        }
    }
    void step4(){
        std::vector<float> tmp(arr.size());
        for(size_t i =0 ; i < arr.size() -1;i++){
            tmp[i] = arr[i - 1] + 2 *arr[i] + arr[i +1 ];
        }
        std::swap(tmp,arr);
    }

};

void normal(std::vector<Data> dats){
    TICK(normal);
    for(auto &dat : dats){
        dat.step1();
        dat.step2();
        dat.step3();
        dat.step4();
    }
    TOCK(normal);
}

//批量处理数据，生产流水线
void tbb_for_each(std::vector<Data> dats){
    TICK(tbb_for_each);
    tbb::parallel_for_each(dats.begin(),dats.end(),[&](Data &dat){
        dat.step1();
        dat.step2();
        dat.step3();
        dat.step4();
    });
    TOCK(tbb_for_each);
}

void tbb_for_each_2(std::vector<Data> dats){
    TICK(tbb_for_each_2);
    tbb::parallel_for_each(dats.begin(),dats.end(),[&](Data &dat){
        dat.step1();
    });
    tbb::parallel_for_each(dats.begin(),dats.end(),[&](Data &dat){
        dat.step2();
    });
    tbb::parallel_for_each(dats.begin(),dats.end(),[&](Data &dat){
        dat.step3();
    });
    tbb::parallel_for_each(dats.begin(),dats.end(),[&](Data &dat){
        dat.step4();
    });
    TOCK(tbb_for_each_2);
}

void tbb_pipeline(std::vector<Data> dats){
    TICK(tbb_pipeline);
    auto it = dats.begin();
    //流水线并行的 filter 参数
    //1. serial_in_order : 表示当前步骤只允许串行执行,且执行的顺序必须一致
    //2. serial_out_of_order : 表示只允许串行执行，但顺序可以打乱
    //3. parallel : 表示可以并行执行当前步骤，且顺序可以被打乱
    tbb::parallel_pipeline(8
    , 
    //TODO:
    //每个步骤的输入和返回参数都可以不一样
    //要求：流水线上一步的返回类型，必须和下一步的输入类型一致
    //且第一步没有输入，最后一步没有返回
    tbb::make_filter<void, Data *>(tbb::filter::mode::serial_in_order,
    [&] (tbb::flow_control &fc) -> Data * {
        if (it == dats.end()) {
            fc.stop();
            return nullptr;
        }
        return &*it++;
    })
    //TODO:
    //'&' 运算符号是为了连接并串联多个过滤器，形成一个并行数据流水线
    & tbb::make_filter<Data *, Data *>(tbb::filter::mode::parallel,
    [&] (Data *dat) -> Data * {
        dat->step1();
        return dat;
    })
    & tbb::make_filter<Data *, Data *>(tbb::filter::mode::parallel,
    [&] (Data *dat) -> Data * {
        dat->step2();
        return dat;
    })
    & tbb::make_filter<Data *, Data *>(tbb::filter::mode::parallel,
    [&] (Data *dat) -> Data * {
        dat->step3();
        return dat;
    })
    & tbb::make_filter<Data *, void>(tbb::filter::mode::parallel,
    [&] (Data *dat) -> void {
        dat->step4();
    })
    );
    TOCK(tbb_pipeline);
}
int main(){
    size_t n = 1 <<11;
    std::vector<Data> dats(n);
    normal(dats);
    tbb_for_each(dats);
    tbb_for_each_2(dats);
    tbb_pipeline(dats);
    return 0;
}
