#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"

static char dir[255] = "./logs";
static int loggerDirSet = 0;
static char filename[128] = "runlog.txt";
static int filenameSet = 0;
static int repeat_in_stdout = 1;
static char out_buffer[512];

void LoggerInitialize() {
    deleteLog();
    setLoggerDirectory("./logs");
    setLoggerFilename("runlog.txt");
    setRepeatInStdout(1);
}

void setLoggerDirectory(const char *directory) {
    snprintf(dir, sizeof(dir), "%s", directory);
    dir[sizeof(dir) - 1] = '\0';
    loggerDirSet = 1;
}

void setLoggerFilename(const char *fname) {
    snprintf(filename, sizeof(filename), "%s", fname);
    filename[sizeof(filename) - 1] = '\0';
    filenameSet = 1;
}

void setRepeatInStdout(int repeat) {
    repeat_in_stdout = repeat;
}

void deleteLog() {
    int ret = 0;
    char dirAndFilename[255] = "\0";

    if (loggerDirSet) {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s", dir, filename);
    } else {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s", filename);
    }
    dirAndFilename[sizeof(dirAndFilename) - 1] = '\0';

    ret = remove(dirAndFilename);

    if (ret == 0) {
        fprintf(stdout, "Log file: %s deleted successfully!\n", filename);
    } else {
        fprintf(stderr, "Error in deleteLog(...): unable to delete the log file: %s\n", filename);
    }
}


void writeToLog(int level, const char *src_filename, const char *function_name, const char *message, ...) {
    FILE *out;
    char dirAndFilename[255] = {0};

    if (loggerDirSet) {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s", dir, filename);
    } else {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s", filename);
    }
    dirAndFilename[sizeof(dirAndFilename) - 1] = '\0';

    out = fopen(dirAndFilename, "a+");

    if (out == NULL) {
        fprintf(stderr, "Error: could not open log file %s for writing\n", dirAndFilename);
        return;
    }

    switch (level) {
        case LOG_INFO: snprintf(out_buffer, sizeof(out_buffer), "%s : Info in %s() - ", src_filename, function_name); break;
        case LOG_WARNING: snprintf(out_buffer, sizeof(out_buffer), "%s : Warning in %s() - ", src_filename, function_name); break;
        case LOG_ERROR: snprintf(out_buffer, sizeof(out_buffer), "%s : Error in %s() - ", src_filename, function_name); break;
        case LOG_FATAL: snprintf(out_buffer, sizeof(out_buffer), "%s : Fatal Error in %s() - ", src_filename, function_name); break;
        case LOG_DEBUG: default: snprintf(out_buffer, sizeof(out_buffer), "%s : Debug in %s() - ", src_filename, function_name); break;
    }

    va_list args;
    va_start(args, message);
    vsnprintf(out_buffer + strlen(out_buffer), sizeof(out_buffer) - strlen(out_buffer), message, args);
    va_end(args);

    fprintf(out, "%s\n", out_buffer);
    fclose(out);

    if (repeat_in_stdout) {
        switch (level) {
            case LOG_INFO: fprintf(stdout, "\033[0;32m"); break;
            case LOG_WARNING: fprintf(stdout, "\033[0;33m"); break;
            case LOG_ERROR: case LOG_FATAL: fprintf(stdout, "\033[0;31m"); break;
            case LOG_DEBUG: default: fprintf(stdout, "\033[0;37m"); break;
        }
        fprintf(stdout, "%s\033[0m\n", out_buffer);
    }

    if (level == LOG_FATAL) {
        exit(1);
    }
}

void writeSeparatorToLog() {
    FILE *out;
    char dirAndFilename[255] = {0};

    if (loggerDirSet) {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s", dir, filename);
    } else {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s", filename);
    }
    dirAndFilename[sizeof(dirAndFilename) - 1] = '\0';

    out = fopen(dirAndFilename, "a+");
    if (out != NULL) {
        fprintf(out, "%s\n", "-------------------------------------------------------------------------------------------------------------------------");
        fclose(out);
    }
}
