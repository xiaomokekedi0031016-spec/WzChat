#pragma once
#include <memory>
#include <mutex>
#include <iostream>
using namespace std;
template <typename T>
class Singleton {
protected:
	Singleton() = default;
	Singleton(const Singleton<T>&) = delete;	
	Singleton& operator=(const Singleton<T>& st) = delete;

public:
	static std::shared_ptr<T> GetInstance() {
		static std::shared_ptr<T> instance(new T);
		return instance;
	}

	static void PrintAddress() {
		std::cout << GetInstance().get() << std::endl;
	}

	~Singleton() {
		std::cout << "this is singleton destruct" << std::endl;
	}

};