#pragma once
#include <stdint.h>

namespace En3rN
{
	namespace Net
	{
		enum class ServerPacket :uint16_t
		{			
			ClientID = 65000, HandShake, Message 	// internal packet types
		};
	}
}

