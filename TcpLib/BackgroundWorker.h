#pragma once
#include <thread>
#include <functional>
#include "tsQue.h"

namespace En3rN
{

	/*class BackgroundWorker
	{
	public:
		BackgroundWorker() = default;
		~BackgroundWorker() = default;
		
		int Stop()
		{
			worker.join();
		}
		void DoWork()
		{
			for (auto work : WorkQue.que)
			{
				work;
				WorkQue.PopFront();
			}
			while (WorkQue.Empty())
				std::this_thread::yield();
		}
	private:
		std::thread worker{ (DoWork, this) };
	public:
		tsQue<std::function<void>> WorkQue;
		
	};*/
}

