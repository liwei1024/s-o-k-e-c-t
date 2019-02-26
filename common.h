#include <windows.h>

unsigned packet_pwd = 123456789;

enum class PacketType
{
	packet_completed,
	packet_read_memory,
	packet_write_memory,
	packet_get_game_status,
};


struct PacketReadMemory
{
	ULONG address;
	SIZE_T size;
};

struct PacketData
{
	void * buffer;
	size_t size;
};

struct PackedCompleted
{
	unsigned int status;
	PacketData   result;
};

struct PacketHeader
{
	unsigned int packet_pwd;
	PacketType   type;
};

struct Packet
{
	PacketHeader header;
	union
	{
		PackedCompleted		completed;
		PacketReadMemory	read_memory;
	}data;
};