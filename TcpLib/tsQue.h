#pragma once
#include "Packet.h"
#include <string>
#include <deque>
#include <mutex>

namespace En3rN::Net
{
		class Packet;
		template <typename T>
		class tsQue 
		{
			
		public:
			tsQue() = default;
			tsQue(tsQue& other) = delete;
			virtual ~tsQue() { Clear(); logger(LogLvl::Debug) << "Queue deleted!"; }


			std::mutex mtxQue;
			std::deque<T> que;

			tsQue& operator << (T& item)
			{
				std::scoped_lock lock(mtxQue);
				que.push_front(item);
				return *this;
			}
						
			tsQue& operator << (T&& item)
			{				
				PushFront(item);
				return *this;
			}

			tsQue& operator >> (tsQue& other)
			{				
				other.PushFront(PopBack());
				return *this;
			}

			const T& Front()
			{
				std::scoped_lock lock(mtxQue);				
				return que.front();
			}
			const T& Back()
			{
				std::scoped_lock lock(mtxQue);				
				return  que.back();
			}

			T PopFront()
			{
				std::scoped_lock lock(mtxQue);
				T item = std::move(que.front());
				que.pop_front();
				return std::move(item);
			}
			T PopBack()
			{
				std::scoped_lock lock(mtxQue);
				T item = std::move(que.back());
				que.pop_back();
				return std::move(item);
			}
			void PushFront(T& item)
			{
				std::scoped_lock lock(mtxQue);
				que.push_front(std::move(item));
			}
			void PushBack(T& item)
			{
				std::scoped_lock lock(mtxQue);
				que.push_back(std::move(item));
			}

			size_t Size()
			{
				return que.size();
			}
			void Clear()
			{
				std::scoped_lock lock(mtxQue);
				que.clear();
			}
			bool Empty()
			{
				return que.empty();
			}
			/*std::iterator<T> Begin()
			{
				return que.begin();
			}
			std::iterator<T> End()
			{
				return que.end();
			}*/

		};
}


