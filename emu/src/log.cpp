#include <common.hpp>
#include <cstring>

Log logger;

Log::Log() {
    // do nothing
}

Log::~Log() {
    if (fp != nullptr) fclose(fp);
}

void Log::setLogFile(const char *filename) {
    if (filename == nullptr) {
        Warn("filename is nullptr");
        return;
    }
    if (fp != nullptr) {
        fclose(fp);
    }
    Debug("set log file to %s", filename);
    fp = fopen(filename, "w");
}

void Log::print(Location loc, LogLevel level, const char *fmt, va_list args) {
#ifdef CONFIG_LOG

    const std::string levelStr[] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR"
    };
    const std::string color[] = {
        ANSI_FG_BLUE,
        ANSI_FG_GREEN,
        ANSI_FG_YELLOW,
        ANSI_FG_RED
    };
    const std::string location = std::string(basename(loc.file));
    const std::string line = std::to_string(loc.line);
    const std::string func = std::string(loc.func);
    const std::string msg = 
        "[" + levelStr[level] + "] " + 
        "[" + location + ":" + line + ":" + func + "] " +
        fmt + "\n";
    const std::string colored_msg = color[level] + msg + ANSI_NONE;
    
    va_list args_copy;
    va_copy(args_copy, args);
    vprintf(colored_msg.c_str(), args);
    if (fp != nullptr) vfprintf(fp, msg.c_str(), args_copy);
#endif
}

void Log::debug(Location loc, const char *fmt, ...) {
#if CONFIG_LOG_LEVEL <= LEVEL_DEBUG
    va_list args;
    va_start(args, fmt);
    print(loc, DEBUG, fmt, args);
    va_end(args);
#endif
}

void Log::info(Location loc, const char *fmt, ...) {
#if CONFIG_LOG_LEVEL <= LEVEL_INFO
    va_list args;
    va_start(args, fmt);
    print(loc, INFO, fmt, args);
    va_end(args);
#endif
}

void Log::warn(Location loc, const char *fmt, ...) {
#if CONFIG_LOG_LEVEL <= LEVEL_WARN
    va_list args;
    va_start(args, fmt);
    print(loc, WARN, fmt, args);
    va_end(args);
#endif
}

void Log::error(Location loc, const char *fmt, ...) {
#if CONFIG_LOG_LEVEL <= LEVEL_ERROR
    va_list args;
    va_start(args, fmt);
    print(loc, ERROR, fmt, args);
    va_end(args);
#endif
}