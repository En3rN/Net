#pragma once
#include <stdint.h>

namespace En3rN
{
	namespace Net
	{
		enum class AppMode
		{
			Loop_Networkthread,			// Loop created within app OnUserUpdate() gets called avery frame, NetworkFrame on a different thread loop
			Loop,						// Loop created within app OnUserUpdate() gets called avery frame, NetworkFrame also gets called every loop
			NetworkFrame,				// no loop from server call NetworFrame() from your program loop when you need networkUpdate, Response comes OnUserUpdate()
			NetworkThread,				// Start() before program loop --> starting networkthread, Response OnUserUpdate()
			
		};
		


	}
}

