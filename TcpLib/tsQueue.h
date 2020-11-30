#pragma once
#include "Packet.h"
#include <string>
#include <deque>
#include <mutex>

namespace En3rN
{
	namespace Net
	{
		class Packet;
		template <typename T>
		class tsQue
		{
			T ptr;
		public:
			tsQue() = default;
			tsQue(tsQue& other) = delete;
			virtual ~tsQue() { Clear(); logger(LogLvl::Debug) << "Queue deleted!"; }


			std::mutex muxQue;
			std::deque<T> que;

			tsQue& operator[] (int index)
			{
				ptr = que[index];
				return *ptr;
			}


			tsQue& operator << (T& item)
			{
				que.push_front(item);
				return *this;

			}
						
			tsQue& operator << (T&& item)
			{
				que.push_front(std::move(item));
				return *this;

			}

			tsQue& operator >> (tsQue& other)
			{
				T item = PopBack();
				other.que.push_front(std::move(item));
				return *this;
			}

			const T& Front()
			{
				std::scoped_lock lock(muxQue);				
				return &que.front();
			}
			const T& Back()
			{
				std::scoped_lock lock(muxQue);				
				return  &que.back();
			}

			T PopFront()
			{
				std::scoped_lock lock(muxQue);
				T item = std::move(que.front());
				que.pop_front();
				return item;
			}
			T PopBack()
			{
				std::scoped_lock lock(muxQue);
				T item = std::move(que.back());
				que.pop_back();
				return item;
			}
			void PushFront(T& item)
			{
				std::scoped_lock lock(muxQue);
				que.push_front(std::move(item));
			}
			void PushBack(T& item)
			{
				std::scoped_lock lock(muxQue);
				que.push_back(std::move(item));
			}

			size_t Size()
			{
				return que.size();
			}
			void Clear()
			{
				std::scoped_lock lock(muxQue);
				que.clear();
			}
			bool Empty()
			{
				return que.empty();
			}
		};
	}
}

