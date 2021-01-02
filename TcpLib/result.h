#pragma once
namespace En3rN::Net
{
	template <typename ReturnType>
	struct Result
	{
		enum class Enum
		{
			Success, Error
		};
		Enum result;
		ReturnType retVal;
	}; 

	
	
}
