#ifndef __LOGGING__H_
#define __LOGGING__H_

#include <sys/time.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__
    #endif
#endif

#define LOG_(_level_, _prefix_, _trace_)                                       \
{                                                                              \
  std::ofstream logfile;                                                       \
  logfile.open("/tmp/ggrep.log", std::ios::out | std::ios::app);               \
  struct timeval __tv;                                                         \
  gettimeofday(&__tv, NULL);                                                   \
  time_t __now = time(0);                                                      \
  struct tm _t;                                                                \
  gmtime_r(&__now, &_t);                                                       \
  char timestamp[255];                                                         \
  strftime(timestamp, 255, "%d/%m/%y %k:%M:%S", &_t);                          \
  logfile << _prefix_ << ",";                                                  \
  logfile << timestamp << "(" << __tv.tv_usec << "),";                         \
  logfile << std::this_thread::get_id() << ",";                                \
  logfile << __FILE__;                                                         \
  logfile << "(" << __LINE__ << "),";                                          \
  logfile << __FUNCTION_NAME__ << ",";                                         \
  logfile << _trace_;                                                          \
  logfile << std::endl;                                                        \
  logfile.close();                                                             \
}

#ifdef S_LOGDBG
# define LOGDBG(_trace_) LOG_(3, "DBG", _trace_);
#else
# define LOGDBG(_trace_)
#endif

#ifdef S_LOGINF
# define LOGINF(_trace_) LOG_(2, "INF", _trace_);
#else
# define LOGINF(_trace_)
#endif

#ifdef S_LOGERR
# define LOGERR(_trace_) LOG_(1, "ERR", _trace_);
#else
# define LOGERR(_trace_)
#endif

class FunLogger {
public:
  FunLogger(const std::string &function_name) : m_function_name(function_name) {
    LOGDBG("Entering " << function_name);
  }
  ~FunLogger() { LOGDBG("Exiting " << m_function_name); }
  std::string m_function_name;
};

#define LOGFN() FunLogger funlogger(__FUNCTION_NAME__);

#endif //__LOGGING__H_