#pragma once
#include <vector>
#include <mutex>

namespace En3rN
{
	template <typename T>
	class tsVec : public std::vector<T> 
	{
	public:
		std::mutex mtx;		

		void PushBack(const T& data)
		{
			std::scoped_lock lock(mtx);
			this->push_back(data);			
		}
	};
}

