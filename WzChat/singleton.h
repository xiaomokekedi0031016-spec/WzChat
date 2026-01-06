#ifndef SINGLETON_H
#define SINGLETON_H

#include "global.h"
#include <iostream>

template<typename T>
class Singleton{
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

public:
    static T& GetInstance(){
        static T obj;
        return obj;
    }

    void PrintAddress() {
        std::cout << &GetInstance() << std::endl;
    }

    ~Singleton(){
        std::cout << "this is singleton destruct" << std::endl;
    }
};

#endif // SINGLETON_H
