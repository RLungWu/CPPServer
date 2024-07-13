#include "log.h"

#include <cctype>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>


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

    LogFormatter::LogFormatter(const std::string& pattern)
        :m_pattern_(pattern){
        
    }

    std::string LogFormatter::format(LogEvent::ptr event){
        std::stringstream ss;
        for(auto& i : m_items_){
            i->format(ss, event);
        }
        return ss.str();
    }


    //%xxx %xxx{xxx} %%
    void LogFormatter::init(){
        std::vector<std::tuple<std::string,std::string, int> > vec;
        std::string nstr;

        for(size_t i = 0; i < m_pattern_.size(); ++i){
            if(m_pattern_[i] != '%' ){
                nstr.append(1, m_pattern_[i]);
                continue;
            }

            if((i+1) < m_pattern_.size()){
                if(m_pattern_[i+1] == '%'){
                    nstr.append(1, '%');
                    continue;
                }
            }


            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;

            while(n < m_pattern_.size()){
                if(!fmt_status && (!isalpha(m_pattern_[n]) && m_pattern_[n] != '{' && m_pattern_[n] != '}')){
                    str = m_pattern_.substr(i + 1, n - i - 1);
                    break;
                }//%xxx
                
                if(fmt_status == 0){
                    if(m_pattern_[n] == '{'){
                        str = m_pattern_.substr(i + 1, n - i - 1);
                        //std:;cout << "*" << str << "*" << std::endl;

                        fmt_status = 1;//解析格式
                        fmt_begin = n;
                        ++n;

                        continue;
                    }
                }else if(fmt_status == 1){
                    if(m_pattern_[n] == '}'){//解析格式结束
                        fmt = m_pattern_.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "{" << fmt << "}" << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }//%{xxx}
                }
                
                ++n;
                
                if(n == m_pattern_.size()){
                    if(str.empty()){
                        str = m_pattern_.substr(i + 1);
                    }
                }

            }

            if(fmt_status == 0){
                if(!nstr.empty()){
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }

                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            }else if(fmt_status == 1){
                std::cout << "pattern parse error: " << m_pattern_ << " - " << m_pattern_[n] << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }
        
        if(!nstr.empty()){
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string& str) >> s_format_items = {
            #define XX(str, C) \
                {#str, [](const std::string& fmt){ return FormatItem::ptr(new C(fmt)); } }

                XX(m, MessageFormatItem),       //消息
                XX(p, LevelFormatItem),         //日志级别
                XX(r, ElapseFormatItem),        //累计毫秒数
                XX(c, NameFormatItem),          //日志名称
                XX(t, ThreadIdFormatItem),      //线程id
                XX(n, NewLineFormatItem),       //换行
                XX(d, DateTimeFormatItem),      //时间
                XX(f, FilenameFormatItem),      //文件名
                XX(l, LineFormatItem),          //行号
                XX(T, TabFormatItem),           //Tab
                XX(F, FiberIdFormatItem)        //协程id
                XX(N, ThreadNameFormatItem)     //线程名称

            #undef XX
        };

        for(auto &i : vec){
            if(std::get<2>(i) == 0){
                m_items_.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }else{
                auto it = s_format_items.find(std::get<0>(i));

                if(it == s_format_items.end()){
                    m_items_.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                }else{
                    m_items_.push_back(it->second(std::get<1>(i)));
                }
            }
        }
    }

    
}//namespace myserver