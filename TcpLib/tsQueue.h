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
		class tsQueue
		{
		public:
			tsQueue() = default;
			tsQueue(tsQueue& other) = delete;
			virtual ~tsQueue() { Clear(); logger(LogLvl::Debug) << "Queue deleted!"; }


			std::mutex muxQueue;
			std::deque<T> Queue;


			tsQueue& operator << (T& item)
			{
				Queue.push_front(item);
				return *this;

			}
						
			tsQueue& operator << (T&& item)
			{
				Queue.push_front(std::move(item));
				return *this;

			}

			tsQueue& operator >> (tsQueue& other)
			{
				T item = PopBack();
				other.Queue.push_front(std::move(item));
				return *this;
			}

			const T& Front()
			{
				std::scoped_lock lock(muxQueue);				
				return &Queue.front();
			}
			const T& Back()
			{
				std::scoped_lock lock(muxQueue);				
				return  &Queue.back();
			}

			T PopFront()
			{
				std::scoped_lock lock(muxQueue);
				T item = std::move(Queue.front());
				Queue.pop_front();
				return item;
			}
			T PopBack()
			{
				std::scoped_lock lock(muxQueue);
				T item = std::move(Queue.back());
				Queue.pop_back();
				return item;
			}
			void PushFront(T& item)
			{
				std::scoped_lock lock(muxQueue);
				Queue.push_front(std::move(item));
			}
			void PushBack(T& item)
			{
				std::scoped_lock lock(muxQueue);
				Queue.push_back(std::move(item));
			}

			size_t Size()
			{
				return Queue.size();
			}
			void Clear()
			{
				std::scoped_lock lock(muxQueue);
				Queue.clear();
			}

		};
	}
}

