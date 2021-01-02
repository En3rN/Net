#pragma once
#include "Connection.h"
#include <vector>
#include <string>
#include "logger.h"
#include <typeinfo>
#include <iterator>
#include "serverpacket.h"


namespace En3rN
{
	namespace Net
	{	
		
		struct Header
		{			
			uint16_t type{};		
			uint16_t packetSize=sizeof(Header);
			uint16_t itemcount=0;
			template <typename t>
			t& PacketType() { return (t&)type; }
		};

		struct ItemHeader
		{
			size_t type{};
			uint16_t size=0;
		};
		

		/*buffer will be stored like this:
		{
		2 byte CustomPacket type		
		2 bytes totalsize
		2 bytes number of items
		}header
		{
		2 byte item type, 2 byte itemsize, item(itemsize)
		2 byte item type, 2 byte itemsize, item(itemsize)
		}body

		*/	
		class Connection;
		class Packet
		{			
		public:			

			static const uint16_t HeaderSize = 6;
			std::shared_ptr<Connection> address;			
			std::vector<char> body;
			

			Packet();			
			Packet(std::shared_ptr<Connection> aOwner);
			template <typename T, class = typename std::enable_if<std::is_enum<T>::value>::type >
			Packet(T packetType , std::shared_ptr<Connection> con = nullptr)
			{
				body.resize(HeaderSize);
				Net::Header header;
				address = con;
				header.type = (uint16_t)packetType;
				header.packetSize = (HeaderSize);
				header.itemcount = 0;
				memcpy(&body[0], &header, HeaderSize);
			}
			Packet(const Packet& other);
			Packet(Packet&& other) noexcept;
			~Packet();
			void Clear();
			void Append(const void* data, uint16_t size);
			/*void WriteHeader();
			void ReadHeader();*/
			Header Header();
			
			uint16_t Size();
			uint16_t Count();
			uint16_t GetItemSize(uint16_t offset);
			size_t GetItemType(uint16_t offset);
			template <typename T>
			size_t SetItemType(T& data)
			{
				return typeid(T).hash_code();
			}
			template<typename T>
			void SetPacketType(T t)
			{
				*(uint16_t*)body.data() = (uint16_t)t;
			}
			
			template <typename T>
			T GetPacketType()
			{
				return (T) * (uint16_t*)&body[0];
			}

			Packet& operator=(const Packet& other);

			Packet& operator >> (std::string& data);
			
			Packet& operator << (const std::string& data);

			template <typename t, class = typename std::enable_if<std::is_standard_layout<t>::value>::type>
			Packet& operator<< (t& data)
			{				
				ItemHeader ih;
				ih.type = SetItemType(data);
				ih.size = sizeof(data);
				
				Append(&ih.type, sizeof(ih.type));
				Append(&ih.size, sizeof(ih.size));
				Append(&data, ih.size);
				//update PacketHeader
				Net::Header header = Header();
				header.itemcount++;
				header.packetSize = (uint16_t)body.size();
				memcpy(&body[0], &header, HeaderSize);
				return *this;
			}
			template <typename t, class = typename std::enable_if<std::is_standard_layout<t>::value>::type>
			Packet& operator>> (t& data)
			{
				uint16_t offset = HeaderSize;
				ItemHeader ih;
				size_t dataType = SetItemType(data);

				while (offset < body.size())
				{
					ih.type = GetItemType(offset);
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


