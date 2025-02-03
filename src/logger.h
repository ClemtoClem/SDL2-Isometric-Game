#ifndef __LOGGER_H
#define __LOGGER_H

#define LOG_DEBUG 1
#define LOG_INFO 2
#define LOG_WARNING 3
#define LOG_ERROR 4
#define LOG_FATAL 5

#include <stdio.h>
#include <stdint.h>

#define LOG_DEFAULT_PATH "./logs"
#define LOG_DEFAULT_FILENAME "runlog"
#define LOG_DEFAULT_EXTENSION "log"

// Define macros
#if defined(_WIN32) || defined(_WIN64)
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define WriteDebug(message, ...)   writeToLog(LOG_DEBUG, __FILENAME__, __LINE__, __func__, message, ##__VA_ARGS__)
#define WriteInfo(message, ...)    writeToLog(LOG_INFO, __FILENAME__, __LINE__, __func__, message, ##__VA_ARGS__)
#define WriteWarning(message, ...) writeToLog(LOG_WARNING, __FILENAME__, __LINE__, __func__, message, ##__VA_ARGS__)
#define WriteError(message, ...)   writeToLog(LOG_ERROR, __FILENAME__, __LINE__, __func__, message, ##__VA_ARGS__)
#define WriteFatal(message, ...)   writeToLog(LOG_FATAL, __FILENAME__, __LINE__, __func__, message, ##__VA_ARGS__)

void LoggerInitialize();
void setLoggerDirectory(const char *directory);
void setLoggerFilename(const char *fname);
void setLoggerExtension(const char *ext);
void setRepeatInStdout(uint8_t repeat);
void setLoggerLevel(uint8_t level);
size_t fileGetLineCount(const char *filename);
void writeToLog(int level, const char *src_filename, size_t line, const char *function_name, const char *message, ...);
void LoggerWriteSeparator();

#endif // __LOGGER_H
