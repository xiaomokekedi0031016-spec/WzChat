#pragma once
#include <iostream>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    static T& Instance() {
        static T obj;  
        return obj;
    }

    static void PrintAddress() {
        std::cout << &Instance() << std::endl;
    }

    ~Singleton() {
        std::cout << "Singleton destructed" << std::endl;
	}   
};
