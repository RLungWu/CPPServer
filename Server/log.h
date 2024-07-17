
#ifndef CPPSERVER_SERVER_LOG_H_
#define CPPSERVER_SERVER_LOG_H_


#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <list>
#include <vector>
#include <sstream>
#include "util.h"
#include "singleton.h"

#define LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        myserver::LogEventWrap(myserver::LogEvent::ptr(new myserver::LogEvent(\
        level, __FILE__, __LINE__, 0, \
        myserver::GetThreadId(), myserver::GetFiberId(), time(0)))).getSS()

#define LOG_DEBUG(logger) LOG_LEVEL(logger, myserver::LogLevel::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, myserver::LogLevel::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, myserver::LogLevel::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, myserver::LogLevel::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, myserver::LogLevel::FATAL)

#define LOG_FMT_LEVEL(logger, level, fmt, ...)\
    if(logger->getLevel() <= level)\
        myserver::LogEventWrap(myserver::LogEvent::ptr(new myserver::LogEvent(\
        level, __FILE__, __LINE__, 0, \
        myserver::GetThreadId(), myserver::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, myserver::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...) LOG_FMT_LEVEL(logger, myserver::LogLevel::INFO, fmt, __VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...) LOG_FMT_LEVEL(logger, myserver::LogLevel::WARN, fmt, __VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, myserver::LogLevel::ERROR, fmt, __VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, myserver::LogLevel::FATAL, fmt, __VA_ARGS__)

#define LOG_ROOT() myserver::LoggerMgr::GetInstance()->getRoot()




namespace myserver{

class Logger;

class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
};



class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>;

    LogEvent(LogLevel::Level level,
            const char* file,
            int32_t m_line,
            uint32_t elapse,
            uint32_t thread_id,
            uint32_t fiber_id,
            uint64_t time);

    const char* getFile() const {return m_file_;}
    int32_t getLine() const {return m_line_;}
    uint32_t getElapse() const {return m_elapse_;}
    uint32_t getThreadId() const {return m_threadId_;}
    uint32_t getFiberId() const {return m_fiberId_;}
    uint64_t getTime() const {return m_time_;}
    std::string getContent() const {return m_ss_.str();}
    std::shared_ptr<Logger> getLogger() const {return m_logger_;}
    LogLevel::Level getLevel() const {return m_level_;}

    std::stringstream& getSS() {return m_ss_;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const char* m_file_ = nullptr;      // File Name
    int32_t m_line_ = 0;                // Line Number
    uint32_t m_elapse_ = 0;             // Time Elapse
    uint32_t m_threadId_ = 0;           //Thread id
    uint32_t m_fiberId_ = 0;            //Fiber id
    uint64_t m_time_ = 0;               //Time
    std::stringstream m_ss_;

    std::shared_ptr<Logger> m_logger_;
    LogLevel::Level m_level_;          //Log Level
};

class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();

    LogEvent::ptr getEvent() const {return m_event_;}
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event_;
};


class LogFormatter{
public:
    using ptr = std::shared_ptr<LogFormatter>;
    
    LogFormatter(const std::string& pattern);
    
    //%t %thread_id %m %n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    //void init();
private:
    class FormatItem{
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem(){};
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };

    void init();

private:
    std::vector<FormatItem::ptr>m_items_;
    std::string m_pattern_;
};

// Log Output Place
class LogAppender{
public:
    using ptr = std::shared_ptr<LogAppender>;
    
    virtual ~LogAppender() {};         //For Inheritance

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    void set_formatter(LogFormatter::ptr val) { m_formatter_ = val; }

    LogFormatter::ptr get_formatter() const { return m_formatter_; }
protected:
    LogLevel::Level m_level_;
    LogFormatter::ptr m_formatter_;
};



// Log Output
class Logger : public std::enable_shared_from_this<Logger>{
public:
    using ptr = std::shared_ptr<Logger>;
    
    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, const LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    
    void info(LogEvent::ptr event);

    void warn(LogEvent::ptr event);

    void fatal(LogEvent::ptr event);

    void error(LogEvent::ptr event);

    void add_appender(LogAppender::ptr appender);

    void delete_appender(LogAppender::ptr appender);

    LogLevel::Level get_level() const { return m_level_; }

    void set_level(LogLevel::Level level) { m_level_ = level; }

    const std::string& getName() const {return m_name_;}
private:
    std::string m_name_;
    LogLevel::Level m_level_;            
    std::list<LogAppender::ptr> m_appenders_;       //Appender List
    LogFormatter::ptr m_formatter;
};

//output to controller
class StdoutLogAppender : public LogAppender{
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
};

// Define appender which will output to file
class FileLogAppender : public LogAppender{
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& filename);
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

    bool reopen();
private:
    std::string m_filename_;
    std::ofstream m_filestream_;
};

class LoggerManager{
    public:
        LoggerManager();
        Logger::ptr getLogger(const std::string& name);

        void init();
        Logger::ptr getRoot() const {return m_root_;}
    private:
        std::map<std::string, Logger::ptr> m_loggers_;
        Logger::ptr m_root_;
};

typedef myserver::Singleton<LoggerManager> LoggerMgr;

}

#endif