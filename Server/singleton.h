#ifndef CPPSERVER_SERVER_SINGLETON_H_
#define CPPSERVER_SERVER_SINGLETON_H_

#include <memory>

namespace myserver{

    template<class T, class X = void, int N = 0>
    class Singleton{
        public:
            static T* GetInstance(){
                static T v;
                return &v;
            }
    };

    template<class T, class X = void, int N = 0>
    class SingletonPtr{
        public:
            static std::shared_ptr<T> GetInstance(){
                static std::shared_ptr<T> v(new T);
                return v;
            }
    };
}
#endif //CPPSERVER_SERVER_SINGLETON_H_


/*  這是一種單利的設計模式，透過singleton，我們可以確保整個server
 * 只有一個instance，這樣可以確保server的資源不會被浪費。
 */