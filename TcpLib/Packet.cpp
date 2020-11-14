#include "Packet.h"
#include "logger.h"
#include <typeinfo>
#include <memory>
#include <assert.h>

namespace En3rN
{
	namespace Net
	{		
		
		Packet::Packet()
		{
			body.resize(sizeof(header));
			address = nullptr;
			header.type = PacketType::Message;
			header.packetSize = (sizeof(header));
			header.itemcount = 0;
			memcpy(&body[0], &header, sizeof(header));
		}

		Packet::Packet(std::shared_ptr<Connection> aOwner, PacketType aType)
		{
			body.resize(sizeof(header));
			body.reserve(100);
			address = aOwner;
			header.type = aType;
			header.packetSize = (sizeof(header));
			header.itemcount = 0;
			memcpy(&body[0], &header, sizeof(header));			
		}
		
		Packet::Packet(const Packet& other)
		{			
			logger(LogLvl::Debug) << "Packet copy!";
			address = other.address;
			header = other.header;
			body = other.body;
		}
		Packet::Packet(Packet&& other) noexcept
		{			
			logger(LogLvl::Debug) << "Packet moved!";
			address = other.address;
			header = other.header;
			body = other.body;
			other.address = nullptr;

			ZeroMemory(&other, sizeof(other));
		}
		Packet::~Packet()
		{			
			if (body.size() != 0) logger(LogLvl::Debug) << "Packet deleted!";
			Clear();

		}
		uint16_t Packet::Size()
		{
			//assert(body.size() > 3)
			return (*(uint16_t*)&body[2]);
		}

		uint16_t Packet::ItemCount()
		{
			return (*(uint16_t*)&body[4]);
		}

		uint16_t Packet::GetItemSize(uint16_t offset)
		{
			return (*(uint16_t*)(body.data()+ (size_t)offset + sizeof(size_t)));
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
				body.insert(body.end(), (char*)data, (char*)data + size);
				header.packetSize += size;
			}
			else
				logger(LogLvl::Error) << "Item does not fit in packet";			
		}

		void Packet::WriteHeader()
		{
			assert(body.size() > sizeof(header.type) + sizeof(header.packetSize) + sizeof(header.itemcount));
			memcpy(body.data(),													&header.type,			sizeof(header.type));
			memcpy(body.data()+ sizeof(header.type),							&header.packetSize,		sizeof(header.packetSize));
			memcpy(body.data()+ sizeof(header.type)+ sizeof(header.packetSize), &header.itemcount,		sizeof(header.itemcount));			
		}

		void Packet::ReadHeader()
		{
			assert(body.size() > sizeof(header.type) + sizeof(header.packetSize) + sizeof(header.itemcount));
			memcpy(&header.type,		body.data(),													sizeof(header.type));
			memcpy(&header.packetSize,	body.data() + sizeof(header.type),								sizeof(header.packetSize));
			memcpy(&header.itemcount,	body.data() + sizeof(header.type) + sizeof(header.packetSize),  sizeof(header.itemcount));
		}

		PacketType Packet::GetPacketType()
		{
			return (PacketType)*(uint16_t*)&body[0];
		}

		Packet& Packet::operator=(const Packet& other)
		{
			logger(LogLvl::Debug) << "packet copy!";
			Clear();
			address = other.address;
			header = other.header;
			body = other.body;
			return *this;
		}
		Packet& Packet::operator>>(std::string& data)
		{
			uint16_t offset = sizeof(header.type)+ sizeof(header.packetSize)+ sizeof(header.itemcount);
			ItemHeader ih;
			size_t dataType = SetType(data);

			while (offset < body.size())
			{
				ih.type = GetType(offset);
				ih.size = GetItemSize(offset);

				if (ih.type == dataType)
				{
					uint16_t itemOffset = offset + sizeof(ih.type) + sizeof(ih.size);
					data.resize(ih.size);
					//data.assign(&body[itemOffset], &body[itemOffset + ih.size]);
					data.assign(body.begin() + offset + sizeof(ih.type) + sizeof(ih.size), 
								body.begin() + offset + sizeof(ih.type) + sizeof(ih.size) + ih.size);
					
					/*ih.type = 0;
					memcpy(&body[offset], &ih.type, sizeof(ih.type));*/
					return *this;
				}
				offset += ih.size + sizeof(ih.type)+ sizeof(ih.size);
			}
			logger(LogLvl::Error) << "Type not found in packet";
			return *this;

		}
		Packet& Packet::operator<<(const std::string& data)
		{
			ItemHeader ih;
			ih.type = SetType(data);
			ih.size = data.size();

			Append(&ih.type, sizeof(ih.type));
			Append(&ih.size, sizeof(ih.size));
			Append(data.data(), data.size());
			
			header.itemcount++;
			WriteHeader();
			
			return *this;
		}
	}
}