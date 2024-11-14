#ifndef __LOGGER_H
#define __LOGGER_H

#define LOG_DEBUG 1
#define LOG_INFO 2
#define LOG_WARNING 3
#define LOG_ERROR 4
#define LOG_FATAL 5

#include <stdio.h>

// Define macros
#if defined(_WIN32) || defined(_WIN64)
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define WriteDebug(message, ...)   writeToLog(LOG_DEBUG, __FILENAME__, __func__, message, ##__VA_ARGS__)
#define WriteInfo(message, ...)    writeToLog(LOG_INFO, __FILENAME__, __func__, message, ##__VA_ARGS__)
#define WriteWarning(message, ...) writeToLog(LOG_WARNING, __FILENAME__, __func__, message, ##__VA_ARGS__)
#define WriteError(message, ...)   writeToLog(LOG_ERROR, __FILENAME__, __func__, message, ##__VA_ARGS__)
#define WriteFatal(message, ...)   writeToLog(LOG_FATAL, __FILENAME__, __func__, message, ##__VA_ARGS__)

void LoggerInitialize();
void setLoggerDirectory(const char *directory);
void setLoggerFilename(const char *fname);
void setRepeatInStdout(int repeat);
void deleteLog();
void writeToLog(int level, const char *src_filename, const char *function_name, const char *message, ...);
void writeSeparatorToLog();

#endif // __LOGGER_H
