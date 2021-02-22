#pragma once
#include "Network.h"

namespace En3rN::Net
{
	template <typename t>
	class TcpBase
	{
	public:
		TcpBase();
		virtual ~TcpBase();

	private:

	};

	template <typename t>
	TcpBase<t>::TcpBase()
	{
	}

	template <typename t>
	TcpBase<t>::~TcpBase()
	{
	}
}
