#pragma once
#include <thread>
#include <functional>

class BackgroundWorker
{
	std::thread worker;
public:
	BackgroundWorker() = default;
	~BackgroundWorker()
	{
		if (!worker.joinable()) worker.detach(); else worker.join();
	}
	template <typename T>
	int DoWork(std::function<T> task)
	{
		task();
	}
};

