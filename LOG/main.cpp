#include "log.h"



int main()
{
    LOGCONFIG(true, "my_loger");
//    LOGCFG(true);
    LOGS << "HEHA" << LOGE;

    int counts= 1000;
    while(counts--){
    LOGINFO("HAHAHAHAH");
    LOGINFO(2);
    LOGWARN(33333);
    LOGERROR("hahasdfasdf");
    }

    return 0;
}