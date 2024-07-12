#include "log.h"

#include <iostream>
#include <string>


namespace myserver{
    Logger::Logger(const std::string& name) : m_name_(name) {}

    void Logger::add_appender(LogAppender::ptr appender){
        m_appenders_.push_back(appender);
    }

    void Logger::delete_appender(LogAppender::ptr appender){
        for(auto it = m_appenders_.begin(); it != m_appenders_.end(); it++){
            if(*it == appender){
                m_appenders_.erase(it);
                break;
            }
        }
    }


    void Logger::log(LogLevel::Level level, const LogEvent::ptr event){
        if(level >= m_level_){
            for(auto& i : m_appenders_){
                i->log(level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event){
        log(LogLevel::DEBUG, event);
    }
    
    void Logger::info(LogEvent::ptr event){
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event){
        log(LogLevel::DEBUG, event);
    }

    void Logger::fatal(LogEvent::ptr event){
        log(LogLevel::DEBUG, event);
    }

    void Logger::error(LogEvent::ptr event){
        log(LogLevel::DEBUG, event);
    }

    FileLogAppender::FileLogAppender(const std::string& filename)
        :m_filestream_(filename){

    }
    
    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
        if(level >= m_level_){
            m_filestream_ << m_formatter_.format(event);
        }
    }

    bool FileLogAppender::reopen(){
        if(m_filestream_){
            m_filestream_.close();
        }
        m_filestream_.open(m_filename_);

        return !!m_filestream_;
    }

    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
        if(level >= m_level_){
            std::cout << m_formatter_.format(event);
        }
    }

}