#pragma once
#include "Connection.h"
#include <vector>
#include <string>
#include "logger.h"
#include <typeinfo>
#include "enumclasses.h"
#include <iterator>


namespace En3rN
{
	namespace Net
	{	
		class Connection;
		enum class PacketType :uint16_t
		{
			Message, Command, ClientID
		};

		/*enum class ItemType : uint16_t
		{
			Integral_Char, Integral_UChar, Integral_Short, Integral_UShort,
			Integral_Int, Integral_UInt, Integral_Long, Integral_ULong,
			Integral_LongLong, Integral_ULongLong, Integral_Float, Integral_Double,
			String, Misc, Extracted
		};*/
				
		
		struct Header
		{
			PacketType type{};
			uint16_t packetSize=sizeof(Header);
			uint16_t itemcount=0;
		};

		struct ItemHeader
		{
			size_t type{};
			uint16_t size=0;
		};

		template <typename t>
		struct Item
		{
			ItemHeader header;
			t data;
		};

		/*buffer will be stored like this:
		{
		2 byte Packet type
		2 bytes totalsize
		2 bytes number of items
		}header
		{
		2 byte item type, 2 byte itemsize, item(itemsize)
		2 byte item type, 2 byte itemsize, item(itemsize)
		}body

		*/
		

		class Packet
		{			
		public:
			//template <typename t>
			//std::iterator<Item<t>> itt;
			std::shared_ptr<Connection> address;
			Header header;
			std::vector<char> body;
			

			Packet();			
			Packet(std::shared_ptr<Connection> aOwner, PacketType aType=PacketType::Message);
			Packet(const Packet& other);
			Packet(Packet&& other) noexcept;
			~Packet();
			void Clear();
			void Append(const void* data, uint16_t size);
			void WriteHeader();
			void ReadHeader();
			PacketType GetPacketType();
			uint16_t Size();
			uint16_t ItemCount();
			uint16_t GetItemSize(uint16_t offset);
			size_t GetType(uint16_t offset);
			template <typename T>
			size_t SetType(T& data)
			{
				return typeid(T).hash_code();
			}			
			

			Packet& operator=(const Packet& other);

			Packet& operator >> (std::string& data);
			
			Packet& operator << (const std::string& data);

			template <typename t, class = typename std::enable_if<std::is_standard_layout<t>::value>::type>
			Packet& operator<< (t& data)
			{				
				ItemHeader ih;
				ih.type = SetType(data);
				ih.size = sizeof(data);
				
				Append(&ih.type, sizeof(ih.type));
				Append(&ih.size, sizeof(ih.size));
				Append(&data, ih.size);
				//update PacketHeader				
				header.itemcount++;
				WriteHeader();
				return *this;
			}
			template <typename t, class = typename std::enable_if<std::is_standard_layout<t>::value>::type>
			Packet& operator>> (t& data)
			{
				uint16_t offset = sizeof(header.type) + sizeof(header.packetSize) + sizeof(header.itemcount);
				ItemHeader ih;
				size_t dataType = SetType(data);


				while (offset < body.size())
				{
					ih.type = GetType(offset);
					ih.size = GetItemSize(offset);

					if (ih.type == dataType)
					{
						//memcpy(&ih, &body[offset], sizeof(ih));
						memcpy(&data, &body[offset + sizeof(ih.type) + sizeof(ih.size)], sizeof(data));
						return *this;
					}
					offset += ih.size + sizeof(ih.type)+sizeof(ih.size);
				}
				logger(LogLvl::Error) << "Type not found in packet";
				return *this;
			}
		};
	}
}


