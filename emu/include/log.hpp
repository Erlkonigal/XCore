#pragma once
#include <cstdio>
#include <cstdarg>

#define Debug(fmt, ...) logger.debug({__FILE__, __LINE__, __func__}, fmt, ##__VA_ARGS__)
#define Info(fmt, ...) logger.info({__FILE__, __LINE__, __func__}, fmt, ##__VA_ARGS__)
#define Warn(fmt, ...) logger.warn({__FILE__, __LINE__, __func__}, fmt, ##__VA_ARGS__)
#define Error(fmt, ...) logger.error({__FILE__, __LINE__, __func__}, fmt, ##__VA_ARGS__)

#define LEVEL_DEBUG 0
#define LEVEL_INFO 1
#define LEVEL_WARN 2
#define LEVEL_ERROR 3

enum LogLevel {
    DEBUG = LEVEL_DEBUG,
    INFO = LEVEL_INFO,
    WARN = LEVEL_WARN,
    ERROR = LEVEL_ERROR
};

struct Location {
    const char *file;
    int line;
    const char *func;
};

class Log {
    FILE *fp;
private:
    void print(Location loc, LogLevel level, const char *fmt, va_list args);
public:
    Log();
    ~Log();
    void setLogFile(const char *filename);
    void debug(Location loc, const char *fmt, ...);
    void info(Location loc, const char *fmt, ...);
    void warn(Location loc, const char *fmt, ...);
    void error(Location loc, const char *fmt, ...);
};

extern Log logger;