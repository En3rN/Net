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
			body.resize(HeaderSize);
			Net::Header header;
			address = nullptr;
			header.type = 0;
			header.packetSize = (HeaderSize);
			header.itemcount = 0;
			memcpy(&body[0], &header, HeaderSize);
		}

		Packet::Packet(std::shared_ptr<Connection> aOwner)
		{
			body.resize(HeaderSize);
			body.reserve(100);
			Net::Header header;
			address = aOwner;
			header.type = (uint16_t) ServerPacket::Message;
			header.packetSize = (HeaderSize);
			header.itemcount = 0;
			memcpy(&body[0], &header, HeaderSize);			
		}
		
		Packet::Packet(const Packet& other)
		{			
			logger(LogLvl::Debug) << "Packet copy!";
			address = other.address;
			body = other.body;
		}
		Packet::Packet(Packet&& other) noexcept
		{			
			logger(LogLvl::Debug) << "Packet moved!";
			address = std::move(other.address);			
			body = std::move(other.body);
			
			other.body.clear();
		}
		Packet::~Packet()
		{			
			if (body.size() != 0) 
				logger(LogLvl::Debug) << "Packet deleted!";
			Clear();
		}
		Header Packet::Header()
		{
			En3rN::Net::Header h;
			uint16_t* ptr = (uint16_t*)body.data();
			h.type = *ptr;
			ptr++;
			h.packetSize = *ptr;
			ptr++;
			h.itemcount = *ptr;
			return h;
		}
		uint16_t Packet::Size()
		{
			//assert(body.size() > 3)
			return (*(uint16_t*)&body[2]);
		}

		uint16_t Packet::Count()
		{
			return (*(uint16_t*)&body[4]);
		}

		uint16_t Packet::GetItemSize(uint16_t offset)
		{
			return (*(uint16_t*)(body.data()+ (size_t)offset + sizeof(size_t)));
		}

		size_t Packet::GetItemType(uint16_t offset)
		{
			return *(size_t*)(&body[offset]);
		}

		void Packet::Clear()
		{			
			body.clear();
		}

		void Packet::Append(const void * data, uint16_t size)
		{
			if (body.size() + size < SO_MAX_MSG_SIZE)
			{				
				body.insert(body.end(), (char*)data, (char*)data + size);				
			}
			else
				logger(LogLvl::Error) << "Item does not fit in packet";			
		}

		//void Packet::WriteHeader()
		//{
		//	assert(body.size() >= HeaderSize);
		//	memcpy(body.data(),													&header.type,			sizeof(header.type));
		//	memcpy(body.data()+ sizeof(header.type),							&header.packetSize,		sizeof(header.packetSize));
		//	memcpy(body.data()+ sizeof(header.type)+ sizeof(header.packetSize), &header.itemcount,		sizeof(header.itemcount));			
		//}

		//void Packet::ReadHeader()
		//{
		//	//assert(body.size() >= sizeof(header.type) + sizeof(header.packetSize) + sizeof(header.itemcount));
		//	memcpy(&header.type,		body.data(),													sizeof(header.type));
		//	memcpy(&header.packetSize,	body.data() + sizeof(header.type),								sizeof(header.packetSize));
		//	memcpy(&header.itemcount,	body.data() + sizeof(header.type) + sizeof(header.packetSize),  sizeof(header.itemcount));
		//}		

		Packet& Packet::operator=(const Packet& other)
		{
			logger(LogLvl::Debug) << "packet copy!";
			Clear();
			address = other.address;			
			body = other.body;
			return *this;
		}
		
		Packet& Packet::operator>>(std::string& data)
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
					uint16_t itemOffset = offset + sizeof(ih.type) + sizeof(ih.size);
					data.resize(ih.size);					
					data.assign(body.begin() + offset + sizeof(ih.type) + sizeof(ih.size), 
								body.begin() + offset + sizeof(ih.type) + sizeof(ih.size) + ih.size);
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
			ih.type = SetItemType(data);
			ih.size = (uint16_t)data.size();

			Append(&ih.type, sizeof(ih.type));
			Append(&ih.size, sizeof(ih.size));
			Append(data.data(), (uint16_t)data.size());
			
			Net::Header header = Header();
			header.itemcount++;
			header.packetSize = (uint16_t)body.size();
			memcpy(&body[0], &header, HeaderSize);
			
			return *this;
		}
	}
}