
#ifndef CPPSERVER_SERVER_INCLUDE_LOG_H_
#define CPPSERVER_SERVER_INCLUDE_LOG_H_


#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <list>



namespace myserver{

class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>;

    LogEvent();
private:
    const char* m_file_ = nullptr;      // File Name
    int32_t m_line_ = 0;                // Line Number
    uint32_t m_elapse_ = 0;             // Time Elapse
    uint32_t m_threadId_ = 0;           //Thread id
    uint32_t m_fiberId_ = 0;            //Fiber id
    uint64_t m_time_ = 0;               //Time
    std::string m_content_;
};

class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

class LogFormatter{
public:
    using ptr = std::shared_ptr<LogFormatter>;
    
    std::string format(LogEvent::ptr event);
private:
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
class Logger{
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
private:
    std::string m_name_;
    LogLevel::Level m_level_;            
    LogAppender::ptr m_appenders_;       //Appender List

};

//output to controller
class StdoutLogAppender : public LogAppender{
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;
    
private:
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





}



#endif