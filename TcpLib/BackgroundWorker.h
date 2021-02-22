//#pragma once
//#include <thread>
//#include <functional>
//#include "tsQue.h"
//
//
//namespace En3rN
//{
//
//	class BackgroundWorker
//	{
//	public:
//		BackgroundWorker() = default;
//		~BackgroundWorker() = default;
//		
//		int Stop()
//		{
//			working=false;
//			worker.join();
//		}
//		void DoWork()
//		{
//			while (working)
//			{
//				for (auto work : WorkQue.que)
//				{
//					work;
//					WorkQue.PopFront();
//				}
//				while (WorkQue.Empty())
//					std::this_thread::yield();
//			}
//			Stop();
//		}
//	private:
//		bool working;
//		std::thread worker{ (DoWork, this) };
//	public:
//		Net::tsQue<std::function<void>> WorkQue;
//		
//	};
//}
//
