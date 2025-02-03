#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "logger.h"

static char dir[255];
static char filename[128];
static char extension[16];
static uint8_t file_setCorrectly = 0; // Bits 0-2 indiquent si dir, filename et extension sont définis
static uint16_t count_files = 0;
static uint8_t repeat_in_stdout = 1;
static uint8_t logger_level = LOG_INFO;
static char out_buffer[1024];
static size_t current_count_lines = 0;
static size_t max_count_lines = 10000;
static uint8_t enable_timestamps = 1;
static char dirAndFilename[405];

/* --------------------------------------------------------------------------------------------------------------------- */
// Static functions prototypes

static size_t fileGetLineCount(const char *filename);
static int makeFilename();

/* --------------------------------------------------------------------------------------------------------------------- */
// Static functions

int makeFilename() {
    if ((file_setCorrectly & 7) != 7) {
        fprintf(stderr, "Logger error: directory, filename or extension is not set!\n");
        return -1;
    }

    if (current_count_lines == 0) {
        if (count_files > 0) {
            snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s.%hu.%s", dir, filename, count_files, extension);
        } else {
            snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s.%s", dir, filename, extension);
        }
        current_count_lines = fileGetLineCount(dirAndFilename);
    }

    while (current_count_lines >= max_count_lines) {
        count_files++;
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s.%hu.%s", dir, filename, count_files, extension);
        current_count_lines = fileGetLineCount(dirAndFilename);
    }

    return 0;
}

size_t fileGetLineCount(const char *filename) {
    int counter = 0;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0; // Retourner 0 si le fichier n'existe pas (éviter -1)
    }

    while (fgets(out_buffer, sizeof(out_buffer), file)) {
        counter++;
    }

    fclose(file);
    return counter;
}

/* --------------------------------------------------------------------------------------------------------------------- */
// Public functions

void LoggerInitialize() {
    LoggerSetDirectory(LOG_DEFAULT_PATH);
    LoggerSetFilename(LOG_DEFAULT_FILENAME);
    LoggerSetExtension(LOG_DEFAULT_EXTENSION);
    LoggerSetLevel(LOG_INFO);
    LoggerSetRepeatInStdout(1);
    LoggerSetTimeStamp(1);
}

void LoggerSetDirectory(const char *directory) {
    snprintf(dir, sizeof(dir), "%s", directory);
    file_setCorrectly |= 1;
    current_count_lines = 0;
}

void LoggerSetFilename(const char *fname) {
    snprintf(filename, sizeof(filename), "%s", fname);
    file_setCorrectly |= 2;
    current_count_lines = 0;
}

void LoggerSetExtension(const char *ext) {
    snprintf(extension, sizeof(extension), "%s", ext);
    file_setCorrectly |= 4;
    current_count_lines = 0;
}

void LoggerSetRepeatInStdout(uint8_t repeat) {
    repeat_in_stdout = repeat;
}

void LoggerSetLevel(uint8_t level) {
    logger_level = level;
}

void LoggerSetTimeStamp(uint8_t enable) {
    enable_timestamps = enable;
}

void LoggerWriteSeparator() {
    if ((file_setCorrectly & 7) != 7) {
        fprintf(stderr, "Logger error: directory, filename or extension is not set!\n");
        return;
    }

    char dirAndFilename[405];
    if (count_files > 0) {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s.%hu.%s", dir, filename, count_files, extension);
    } else {
        snprintf(dirAndFilename, sizeof(dirAndFilename), "%s/%s.%s", dir, filename, extension);
    }

    FILE *out = fopen(dirAndFilename, "a");
    if (out != NULL) {
        fprintf(out, "----------------------------------------------------------------------------------------------------\n");
        fclose(out);
    } else {
        fprintf(stderr, "Logger error: could not open file %s for writing\n", dirAndFilename);
    }
}

void writeToLog(int level, const char *src_filename, size_t line, const char *function_name, const char *message, ...) {
    if (logger_level > level || makeFilename() != 0) {
        return;
    }

    FILE *out = fopen(dirAndFilename, "a");
    if (out == NULL) {
        fprintf(stderr, "Logger error: could not open file '%s' for writing\n", dirAndFilename);
        return;
    }

    const char *levelStr;
    switch (level) {
        case LOG_INFO: levelStr = "Info"; break;
        case LOG_WARNING: levelStr = "Warning"; break;
        case LOG_ERROR: levelStr = "Error"; break;
        case LOG_FATAL: levelStr = "Fatal"; break;
        case LOG_DEBUG:
        default: levelStr = "Debug"; break;
    }

    if (enable_timestamps) {
        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        snprintf(out_buffer, sizeof(out_buffer), "%s [%s] %s:%lu %s() - ", timestamp, levelStr, src_filename, line, function_name);
    } else {
        snprintf(out_buffer, sizeof(out_buffer), "[%s] %s:%lu %s() - ", levelStr, src_filename, line, function_name);
    }

    va_list args;
    va_start(args, message);
    vsnprintf(out_buffer + strlen(out_buffer), sizeof(out_buffer) - strlen(out_buffer), message, args);
    va_end(args);

    fprintf(out, "%s\n", out_buffer);
    fclose(out);
    current_count_lines++;

    if (repeat_in_stdout) {
        const char *color;
        switch (level) {
            case LOG_INFO: color = "\033[0;32m"; break;
            case LOG_WARNING: color = "\033[0;33m"; break;
            case LOG_ERROR:
            case LOG_FATAL: color = "\033[0;31m"; break;
            case LOG_DEBUG:
            default: color = "\033[0;37m"; break;
        }
        fprintf(stdout, "%s%s\033[0m\n", color, out_buffer);
    }

    if (level == LOG_FATAL) {
        exit(EXIT_FAILURE);
    }
}