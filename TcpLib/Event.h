#pragma once
#include <string>
namespace En3rN
{
	class Event
	{
		enum class Type
		{
			Keyboard, Mouse, GameUpdate
		};

		virtual std::string ToString() = 0;
	};
}

