#pragma once
#include <iostream>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    //static T& GetInstance() {
    //    static T obj;  
    //    return obj;
    //}
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> instance(new T);
        return instance;
    }

    static void PrintAddress() {
        std::cout << GetInstance().get() << std::endl;
    }

    ~Singleton() {
        std::cout << "Singleton destructed" << std::endl;
    }
};
