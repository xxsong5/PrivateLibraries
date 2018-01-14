#include "log.h"
#include <thread>


int other(int a){

    int counts= 1000;
    LOGS << a << LOGE;
    while(counts--){
        LOGWARN("other");
    }
}



int main()
{


    LOGCONFIG(true, "my_loger");
//    LOGCFG(true);
    LOGS << "HEHA" << LOGE;

    int a = 9;
    std::thread  athread(other, a);

    int counts= 1000;
    while(counts--){
    LOGINFO("HAHAHAHAH");
    LOGINFO(2);
    LOGWARN(33333);
    LOGERROR("hahasdfasdf");
    }

    athread.join();

    return 0;
}