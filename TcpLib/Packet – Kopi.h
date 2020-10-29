#pragma once
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <WinSock2.h>
#include <string>
#include "logger.h"

enum class PacketType :uint8_t
{
	Message,Command

};

enum class ItemType :uint8_t
{
	Integral_Char, Integral_UChar, Integral_Short, Integral_UShort,
	Integral_Int, Integral_UInt, Integral_Long, Integral_ULong,
	Integral_LongLong, Integral_ULongLong, Integral_Float, Integral_Double,
	String, Extracted
};

template <typename packetType> 
struct Header
{
	packetType pType{};
	uint16_t packetSize=sizeof(Header);
	uint16_t itemcount=0;
};

template<typename DataType>
struct Item
{
	ItemType itemenum;
	uint16_t itemsize;
	DataType data;
};

/*buffer will be stored like this:
	{
	1 byte Packet type
	2 bytes totalsize
	2 bytes number of items
	}header
	{
	1 byte item type, 2 byte itemsize, item(itemsize)
	1 byte item type, 2 byte itemsize, item(itemsize)
	}body

*/
class Packet
{
	static const uint16_t ItemHeaderSize = 3;
	static const uint16_t PacketHeaderSize = 4;
	uint16_t Offset = PacketHeaderSize;
public:
	SOCKET socket;
	std::string buffer;

public:
	Packet();
	Packet(SOCKET& socket);
	Packet(const Packet& other);
	Packet(Packet&& other) noexcept;	
	~Packet();
	void Clear();
	void Append(const void* data, uint16_t size);	
	uint16_t Size();
	uint16_t ItemCount();
	uint16_t GetItemSize(uint16_t offset);
	ItemType GetType(uint16_t offset);
	Packet& operator << (std::string& data);
	Packet& operator >> (std::string& data);
	Packet& operator<<(const char* data);
	Packet& operator=(const Packet& other);

	template <typename t>
	Packet& operator<< (t data);
	/*{
		ItemType itemtype = SetType(data);
		if (itemtype != ItemType::String)
		{			
			uint16_t nsize = 0;
			nsize = sizeof(t);
			uint8_t itemType = (uint8_t)itemtype;
			Append(&itemtype, sizeof(uint8_t));
			Append(&nsize, sizeof(uint16_t));
			Append(&data, sizeof(t));
			uint16_t* pItemCount = (uint16_t*)buffer.data() + 1;
			*pItemCount = (uint16_t)((ItemCount() + 1));
		}
		return *this;
	}*/	
	template <typename t>	
	Packet& operator>> (t& data);
	/*{
		if (typeid(t) == typeid(std::string)) return *this;

		while (Offset < buffer.size())
		{

			ItemType itemFromBufType = GetType(Offset);
			ItemType outputType = SetType(data);
			uint16_t size = GetItemSize(Offset);
			if (outputType == itemFromBufType)
			{	
				data = GetIntegralItem<t>(Offset);				
				uint8_t* itemtype = (uint8_t*)buffer.data() + Offset;
				*itemtype = (uint8_t)ItemType::Extracted;				
				return *this;
			}
			Offset += ItemHeaderSize + size;
		}
		logger(LogLvl::Error) << "Type not found in packet";
		return *this;
	}*/

	template <typename t>
	t operator[](uint16_t itemID)
	{
		uint16_t id = 0;
		if (itemID > ItemCount())
		{
			logger(LogLvl::Error) << "[" << itemID << "] Doesnt exist!";
			return (int*)-1;
		}
		Offset = PacketHeaderSize;
		while (Offset < Size())
		{
			ItemType Type;
			uint16_t ItemSize;

			if (id == itemID)
			{
				Type = GetType(Offset);
				ItemSize = GetItemSize(Offset);
				//t item = GetItem(Offset, Type, ItemSize);
				return t();
			}
			Offset += ItemHeaderSize + ItemSize;
			id++;
		}
	}
	template <typename t>
	t GetIntegralItem(uint16_t Offset)
	{		
			t* item = (t*)&buffer[(size_t)Offset + ItemHeaderSize];
			return *item;
	}
	

	template <typename t>
	ItemType SetType(const t& data)
	{
		if (typeid(t) == typeid(char))					return ItemType::Integral_Char;
		if (typeid(t) == typeid(unsigned char))			return ItemType::Integral_UChar;
		if (typeid(t) == typeid(short))					return ItemType::Integral_Short;
		if (typeid(t) == typeid(unsigned short))		return ItemType::Integral_UShort;
		if (typeid(t) == typeid(int))					return ItemType::Integral_Int;
		if (typeid(t) == typeid(unsigned int))			return ItemType::Integral_UInt;
		if (typeid(t) == typeid(long))					return ItemType::Integral_Long;
		if (typeid(t) == typeid(unsigned long))			return ItemType::Integral_ULong;
		if (typeid(t) == typeid(long long))				return ItemType::Integral_LongLong;
		if (typeid(t) == typeid(unsigned long long))	return ItemType::Integral_ULongLong;
		if (typeid(t) == typeid(float))					return ItemType::Integral_Float;
		if (typeid(t) == typeid(double))				return ItemType::Integral_Double;
		
		return ItemType::String;
	}
	template <typename t>
	uint16_t hToN(t num)
	{
		logger(LogLvl::Debug) << "SizeofType: " << sizeof(t);
		if (typeid(t) == typeid(char))					return num;
		if (typeid(t) == typeid(unsigned char))			return num;
		if (typeid(t) == typeid(short))					return htons(num);
		if (typeid(t) == typeid(unsigned short))		return htons(num);
		if (typeid(t) == typeid(int))					return htoni(num);
		if (typeid(t) == typeid(unsigned int))			return htoni(num);
		if (typeid(t) == typeid(long))					return htonl(num);
		if (typeid(t) == typeid(unsigned long))			return htonl(num);
		if (typeid(t) == typeid(long long))				return htonll(num);
		if (typeid(t) == typeid(unsigned long long))	return htonll(num);
		if (typeid(t) == typeid(float))					return htonf(num);
		if (typeid(t) == typeid(double))				return htond(num);

		return num;
	}

	template <typename t>
	uint16_t nToH(t num)
	{
		logger(LogLvl::Debug) << "SizeofType: " << sizeof(t)  << " Itemtype:" << typeid(t).name();
		if (typeid(t) == typeid(char))					return num;
		if (typeid(t) == typeid(unsigned char))			return num;
		if (typeid(t) == typeid(short))					return ntohs(num);
		if (typeid(t) == typeid(unsigned short))		return ntohs(num);
		if (typeid(t) == typeid(int))					return ntohi(num);
		if (typeid(t) == typeid(unsigned int))			return ntohi(num);
		if (typeid(t) == typeid(long))					return ntohl(num);
		if (typeid(t) == typeid(unsigned long))			return ntohl(num);
		if (typeid(t) == typeid(long long))				return ntohll(num);
		if (typeid(t) == typeid(unsigned long long))	return ntohll(num);
		if (typeid(t) == typeid(float))					return ntohf(num);
		if (typeid(t) == typeid(double))				return ntohd(num);
		return num;;
	}
};

