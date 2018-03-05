#ifndef __XUXIANSONG_LOGGER__
#define __XUXIANSONG_LOGGER__

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <mutex>
#include <thread>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>



class Logger {

public:
    static Logger* getInstance(){
        
        std::lock_guard<std::mutex>  lck(Logger::x_mutex);
        if (NULL == Logger::pInstance){
            Logger::pInstance = new Logger;
        }
        return Logger::pInstance;
    }

    static void destroyInstance(){
        if (NULL != Logger::pInstance){
            delete Logger::pInstance;
            Logger::pInstance = NULL;
        }
    }


    void Config(bool isToFile= false, std::string filePrefix="log"){
        m_btoFile           = isToFile;
        m_filePrifix        = filePrefix;
    }


    bool isWrite2File(){
        return m_btoFile;
    }


    void write2File(std::string strlog){

        if (access(m_logdir,R_OK|W_OK) < 0)
            mkdir(m_logdir,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);

        std::string fileName =std::string(m_logdir) + "/" + m_filePrifix + "_" + FNGenerator();

        {//fresh
            static int counts = 0;
            ++counts;
            if (counts > 100){
                counts = 0;
                if (m_pfile){
                    fflush(m_pfile);
                }
            }
            if (m_preFileName != fileName){
                if (m_pfile){
                    fflush(m_pfile);
                    fclose(m_pfile);
                    m_pfile = NULL;
                }
                m_preFileName = m_preFileName != fileName ? fileName : m_preFileName;
            }
        }

        if (NULL == m_pfile){
            m_pfile = fopen(fileName.c_str(), "wb+");
        }

        if (NULL != m_pfile){
            fwrite(strlog.c_str(), strlog.length(), 1, m_pfile);
            fflush(m_pfile);
        }

    }

    std::mutex& getMutex(){
        return Logger::x_mutex;
    }

    std::string gettid(){
        char strIds[100] = {};
        int pid = (int)getpid();
        int tid = (int)syscall(__NR_gettid);
        sprintf(strIds, "%08d@@%08d",pid, tid);
        return  std::string(strIds);
    }

    char* getMem1000(){return m_HeadInfo;}

    ~Logger(){
        if (m_pfile){
            fflush(m_pfile);
            fclose(m_pfile);
            m_pfile = NULL;
        }
    }

private:
    Logger(bool toFile = false, std::string filePrefix = "log"): m_btoFile(toFile),
     m_logdir("LOG"), m_pfile(NULL), m_filePrifix(filePrefix), m_preFileName(""){ }
    Logger(const Logger&) = delete;
    Logger& operator = (const Logger &);

    std::string FNGenerator(){
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime (buffer,80,"%F",timeinfo);
        return std::string(buffer);
    }

private:
    static Logger       *pInstance;
    static std::mutex    x_mutex;

    bool                 m_btoFile;
    const char          *m_logdir;
    FILE                *m_pfile;
    std::string          m_filePrifix;
    std::string          m_preFileName;
    char                 m_HeadInfo[1000];
};

#define     __GREEN_COLOR__     "\033[0;32m"
#define     __LGREEN_COLOR__     "\033[0;32m"
#define     __RED_COLOR__       "\033[0;31m"
#define     __LRED_COLOR__       "\033[0;31m"
#define     __YELLOW_COLOR__    "\033[0;33m"
#define     __LYELLOW_COLOR__    "\033[0;33m"
#define     __CLOSE_COLOR__     "\033[0m"


#define LOGCONFIG(is2File, filePrefix)  do { \
bool _2File = (bool)is2File; \
std::string fileP = std::string(filePrefix); \
Logger *_logger = Logger::getInstance(); \
_logger->Config(_2File, fileP); \
}while(0) \


#define LOGCFG(is2File)  do { \
bool _2File = (bool)is2File; \
Logger *_logger = Logger::getInstance(); \
_logger->Config(_2File); \
}while(0) \


#define LOGINFO(astring)    do{ \
Logger * _logger = Logger::getInstance(); \
if (_logger){ \
    char headInfo[1000]={}; \
    sprintf(headInfo, "[LOGINFO__%s__%s__%06d]", _logger->gettid().c_str(), __FILE__, __LINE__); \
    std::lock_guard<std::mutex>    lck(_logger->getMutex()); \
    if (!_logger->isWrite2File()) { \
        std::cout << __LGREEN_COLOR__ << std::string(headInfo) << __GREEN_COLOR__ << astring << __CLOSE_COLOR__ << std::endl; \
    }else { \
        std::stringstream ss; \
        ss << headInfo; \
        ss << astring; \
        ss << "\n"; \
        _logger->write2File(ss.str()); \
    } \
} \
}while(0)  \


#define LOGWARN(astring)    do{ \
Logger * _logger = Logger::getInstance(); \
if (_logger){ \
    char headInfo[1000]={}; \
    sprintf(headInfo, "[LOGWARN__%s__%s__%06d]", _logger->gettid().c_str(), __FILE__, __LINE__); \
    std::lock_guard<std::mutex>    lck(_logger->getMutex()); \
    if (!_logger->isWrite2File()) { \
        std::cout << __LYELLOW_COLOR__ << std::string(headInfo) << __YELLOW_COLOR__ << astring << __CLOSE_COLOR__ << std::endl; \
    }else { \
        std::stringstream ss; \
        ss << headInfo; \
        ss << astring; \
        ss << "\n"; \
        _logger->write2File(ss.str()); \
    } \
} \
}while(0)  \

#define LOGERROR(astring)    do{ \
Logger * _logger = Logger::getInstance(); \
if (_logger){ \
    char headInfo[1000]={}; \
    sprintf(headInfo, "[LOGERROR__%s__%s__%06d]", _logger->gettid().c_str(), __FILE__, __LINE__); \
    std::lock_guard<std::mutex>    lck(_logger->getMutex()); \
    if (!_logger->isWrite2File()) { \
        std::cout << __LRED_COLOR__ << std::string(headInfo) << __RED_COLOR__ << astring << __CLOSE_COLOR__ << std::endl; \
    }else { \
        std::stringstream ss; \
        ss << headInfo; \
        ss << astring; \
        ss << "\n"; \
        _logger->write2File(ss.str()); \
    } \
} \
}while(0)  \




#define LOGS  \
    if (Logger::getInstance()) \
        sprintf(Logger::getInstance()->getMem1000(), "[LOG__%s__%s__%06d]", Logger::getInstance()->gettid().c_str(), __FILE__, __LINE__); \
    std::cout << __LGREEN_COLOR__ << std::string(Logger::getInstance()->getMem1000()) << __GREEN_COLOR__

#define LOGE \
     __CLOSE_COLOR__ << std::endl;







#endif
