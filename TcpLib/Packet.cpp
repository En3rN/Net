#include "Packet.h"
#include "logger.h"
#include <typeinfo>
#include <memory>

namespace En3rN
{
	namespace Net
	{		
		
		Packet::Packet()
		{
			body.resize(sizeof(header));
			owner = nullptr;
			header.type = PacketType::Message;
			header.packetSize = (sizeof(header));
			header.itemcount = 0;
			memcpy(&body[0], &header, sizeof(header));
		}

		Packet::Packet(std::shared_ptr<Connection> aOwner, PacketType aType)
		{
			body.resize(sizeof(header));
			owner = aOwner;
			header.type = aType;
			header.packetSize = (sizeof(header));
			header.itemcount = 0;
			memcpy(&body[0], &header, sizeof(header));
		}
		
		Packet::Packet(const Packet& other)
		{
			//logger(LogLvl::Debug) << "Packet copy!";
			owner = other.owner;
			header = other.header;
			body = other.body;
		}
		Packet::Packet(Packet&& other) noexcept
		{
			//logger(LogLvl::Debug) << "Packet moved!";
			owner = other.owner;
			header = other.header;
			body = other.body;
			other.owner = nullptr;

			ZeroMemory(&other, sizeof(other));
		}
		Packet::~Packet()
		{
			//logger(LogLvl::Debug) << "Packet deleted!";
			Clear();

		}
		uint16_t Packet::Size()
		{
			return (*(uint16_t*)&body[2]);
		}

		uint16_t Packet::ItemCount()
		{
			return (*(uint16_t*)&body[4]);
		}

		uint16_t Packet::GetItemSize(uint16_t offset)
		{
			return (*(uint16_t*)(&body[(size_t)offset + sizeof(ItemHeader::type)]));
		}

		size_t Packet::GetType(uint16_t offset)
		{
			return *(size_t*)(&body[offset]);
		}

		void Packet::Clear()
		{
			ZeroMemory(&header, sizeof(header));
			body.clear();
		}

		void Packet::Append(const void * data, uint16_t size)
		{
			if (body.size() + size < SO_MAX_MSG_SIZE)
			{				
				body.insert(body.end(), (char)data, (char)data + size);
			}
			else
				logger(LogLvl::Error) << "Item does not fit in packet";			
		}

		PacketType Packet::GetPacketType()
		{
			return (PacketType)*(uint16_t*)&body[0];
		}

		Packet& Packet::operator=(const Packet& other)
		{
			logger(LogLvl::Debug) << "packet copy!";
			Clear();
			owner = other.owner;
			header = other.header;
			body = other.body;
			return *this;
		}
		Packet& Packet::operator>>(std::string& data)
		{
			uint16_t offset = sizeof(header);
			ItemHeader itemheader;
			size_t dataType = SetType(data);

			while (offset < body.size())
			{
				itemheader.type = GetType(offset);
				itemheader.size = GetItemSize(offset);

				if (itemheader.type == dataType)
				{
					data.resize(itemheader.size);
					data.assign(&body[offset + sizeof(itemheader)], &body[offset + sizeof(itemheader) + itemheader.size]);
					memcpy(&body[offset], &itemheader.type, sizeof(itemheader.type));
					return *this;
				}
				offset += itemheader.size + sizeof(itemheader);
			}
			logger(LogLvl::Error) << "Type not found in packet";
			return *this;

		}
		Packet& Packet::operator<<(std::string& data)
		{
			ItemHeader ih;
			ih.type = SetType(data);
			ih.size = data.size();
			body.resize(body.size() + sizeof(ih));
			memcpy(&body[header.packetSize], &ih, sizeof(ih));
			body.insert(body.size(), data);
			header.packetSize += sizeof(ih) + ih.size;
			header.itemcount++;
			memcpy(&body[0], &header, sizeof(header));
			
			return *this;
		}
	}
}


//template Packet& Packet::operator<< <char>					(char data);
//template Packet& Packet::operator<< <unsigned char>			(unsigned char data);
//template Packet& Packet::operator<< <short>					(short data);
//template Packet& Packet::operator<< <unsigned short>		(unsigned short data); 
//template Packet& Packet::operator<< <int>					(int data);
//template Packet& Packet::operator<< <unsigned int>			(unsigned int data);
//template Packet& Packet::operator<< <long>					(long data);
//template Packet& Packet::operator<< <unsigned long>			(unsigned long data);
//template Packet& Packet::operator<< <long long>				(long long data);
//template Packet& Packet::operator<< <unsigned long long>	(unsigned long long	data);
//template Packet& Packet::operator<< <float>					(float data);
//template Packet& Packet::operator<< <double>				(double data);
//
//template <typename t>
//Packet& Packet::operator<< (t data)
//{
//	ItemType itemtype = SetType(data);
//	if (itemtype != ItemType::String)
//	{		
//		uint16_t nsize = 0;
//		nsize = sizeof(t);
//		uint8_t itemType = (uint8_t)itemtype;
//		Append(&itemtype, sizeof(uint8_t));
//		Append(&nsize, sizeof(uint16_t));
//		Append(&data, sizeof(t));
//		uint16_t* pItemCount = (uint16_t*)buffer.data() + 1;
//		*pItemCount = (uint16_t)((ItemCount() + 1));
//	}
//	return *this;
//}

//template Packet& Packet::operator>> <char>					(char& data);
//template Packet& Packet::operator>> <unsigned char>			(unsigned char& data);
//template Packet& Packet::operator>> <short>					(short& data);
//template Packet& Packet::operator>> <unsigned short>		(unsigned short& data);
//template Packet& Packet::operator>> <int>					(int& data);
//template Packet& Packet::operator>> <unsigned int>			(unsigned int& data);
//template Packet& Packet::operator>> <long>					(long& data);
//template Packet& Packet::operator>> <unsigned long>			(unsigned long& data);
//template Packet& Packet::operator>> <long long>				(long long& data);
//template Packet& Packet::operator>> <unsigned long long>	(unsigned  long long& data);
//template Packet& Packet::operator>> <float>					(float& data);
//template Packet& Packet::operator>> <double>				(double& data);
//
//template <typename t>
//Packet& Packet::operator>>(t& data)
//{
//	//if (typeid(t) == typeid(std::string)) return *this;
//
//	while (Offset < buffer.size())
//	{
//
//		ItemType itemFromBufType = GetType(Offset);
//		ItemType outputType = SetType(data);
//		uint16_t size = GetItemSize(Offset);
//		if (outputType == itemFromBufType)
//		{
//			data = GetIntegralItem<t>(Offset);
//			uint8_t* itemtype = (uint8_t*)buffer.data() + Offset;
//			*itemtype = (uint8_t)ItemType::Extracted;
//			return *this;
//		}
//		Offset += ItemHeaderSize + size;
//	}
//	logger(LogLvl::Error) << "Type[" << typeid(t).name() <<"] not found in packet";
//	return *this;
//}

//Packet::Packet()
//{
//	Clear();
//	buffer.resize(sizeof(header));
//	socket = INVALID_SOCKET;
//	header.type = PacketType::Message;
//	header.packetSize = (sizeof(header));
//	header.itemcount = 0;
//	memcpy(&buffer[0], &header, sizeof(header));
//}

