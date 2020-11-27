#pragma once
#include <stdint.h>

namespace En3rN
{
	namespace Net
	{
		enum class PacketType :uint16_t
		{
			Message,
			ClientID = 65000, HandShake		// internal packet types
		};
	}
}

