#include <cstdint>
#include <string>
#include <memory>
#include <iostream>



class LogLevel{
    public:
        enum Level{
            UNKNOWN = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5,
        };
};

class LogEvent{
    public:
        using ptr = std::shared_ptr<LogEvent>;
        LogEvent(LogLevel::Level level,
                const char* file,
                int32_t m_line,
                uint32_t elapse,
                uint32_t trhead_id,
                uint32_t fiber_id,
                uint64_t time);
        
        const char* getFile() const {return m_file; }
        int32_t getLine() const {return m_line; }
        uint32_t getElapse() const {return m_elapse; }
        uint32_t getThreadId() const {return m_threadId; }
        uint32_t getFiberId() const {return m_fiberId; }
        uint64_t getTime() const {return m_time; }
        LogLevel::Level getLevel() const {return m_level; }
    
    private:
        const char* m_file = nullptr;       //文件名
        int32_t m_line = 0;                 //行号
        uint32_t m_elapse = 0;              //程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;            //線程id
        uint32_t m_fiberId = 0;             //協程id
        uint64_t m_time;                    //時間戳
        LogLevel::Level m_level;            //日志等级
};

LogEvent::LogEvent(LogLevel::Level level,
                const char* file,
                int32_t m_line,
                uint32_t elapse,
                uint32_t trhead_id,
                uint32_t fiber_id,
                uint64_t time)
    : m_level(level)
    , m_file(file)
    , m_line(m_line)
    , m_elapse(elapse)
    , m_threadId(trhead_id)
    , m_fiberId(fiber_id)
    , m_time(time) {
}

class Logger{
    public:
        using ptr = std::shared_ptr<Logger>;
        
        Logger(const std::string& name = "root");

        const std::string& getName() const {return m_name; }
        LogLevel::Level getLevel() const {return m_level; }
        void setLevel(LogLevel::Level level) {m_level = level; }

        void log(LogEvent::ptr event);


    private:
        std::string m_name;
        LogLevel::Level m_level;
};

Logger::Logger(const std::string& name)
    : m_name(name),
      m_level(LogLevel::DEBUG){
}

void Logger::log(LogEvent::ptr event){
    if(event->getLevel() >= m_level){
        std::cout << "輸出模擬:" << std::endl;
    }
}

class LogAppender{
    public:
        using ptr = std::shared_ptr<LogAppender>;
        virtual ~LogAppender(){};
        virtual void log(LogEvent::ptr event) = 0;
};

class StdoutLogAppender :public LogAppender{
    public:
        using ptr = std::shared_ptr<StdoutLogAppender>;
        void log(LogEvent::ptr event) override;
};

void StdoutLogAppender::log(LogEvent::ptr event){
    std::cout
        << event->getTime() << " "
        << event->getThreadId() << " "
        << event->getFiberId() << " "
        << "["
        << event->getElapse() << " "
        << "] "
        << event->getFile() << ":" << event->getLine() << " "
        << "輸出到控制台的訊息"
        << std::endl;
}

class FileLogAppender :public LogAppender{
    public:
        using ptr = std::shared_ptr<FileLogAppender>;
        FileLogAppender(const std::string& filename);
        void log(LogEvent::ptr event) override;
    private:
        std::string m_filename;
};

FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename(filename){

}

void FileLogAppender::log(LogEvent::ptr event){
    std::cout << "輸出到文件:" << m_filename << std::endl;
}



int main(){
    LogEvent::ptr event(new LogEvent(LogLevel::DEBUG, "log_test.cc", 32, 1000, 1, 2, 3));
    Logger::ptr logger(new Logger("XYZ"));
    logger->log(event);

    logger->setLevel(LogLevel::ERROR);
    
    return 0;
}