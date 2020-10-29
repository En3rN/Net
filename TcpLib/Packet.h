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
			t item;
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
			std::shared_ptr<Connection> owner;
			Header header;
			std::string body;

			Packet();			
			Packet(std::shared_ptr<Connection> aOwner, PacketType aType=PacketType::Message);
			Packet(const Packet& other);
			Packet(Packet&& other) noexcept;
			~Packet();
			void Clear();
			void Append(const void* data, uint16_t size);
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
			
			uint16_t ItemSize(char* data)	
			{				
				return sizeof(data);
			}

			Packet& operator=(const Packet& other);

			Packet& operator >> (std::string& data);
			
			Packet& operator << (std::string& data);

			template <typename t>
			Packet& operator<< (const t& data)
			{
				ItemHeader ih;
				ih.type = SetType(data);
				ih.size = ItemSize((char*)data);
				
				
				body.resize(body.size() + sizeof(ih) + sizeof(data));
				memcpy(&body[header.packetSize], &ih, sizeof(ih)); 
				memcpy(&body[header.packetSize + sizeof(ih)], &data, sizeof(data));
				/*Append((char*)ih.type, sizeof(ih.type));
				Append((char*)ih.size, sizeof(ih.size));
				Append((char*)data, ih.size);*/

				//update PacketHeader
				header.packetSize += sizeof(ih) + ih.size;
				header.itemcount++;
				memcpy(&body[0], &header, sizeof(header));
				return *this;
			}
			template <typename t>
			Packet& operator>> (t& data)
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
						memcpy(&itemheader, &body[offset], sizeof(itemheader));
						memcpy(&data, &body[offset + sizeof(itemheader)], sizeof(data));						
						return *this;
					}
					offset += itemheader.size + sizeof(itemheader);
				}
				logger(LogLvl::Error) << "Type not found in packet";
				return *this;
			}
		};
	}
}


