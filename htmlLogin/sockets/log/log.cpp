#include "log.h"

Logger* Logger::pInstance = NULL;
std::mutex Logger::x_mutex;

