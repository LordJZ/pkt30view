#pragma once
#include <Windows.h>

#pragma pack(push, 1)

struct PKT_HEADER
{
	BYTE signature[3]; // 'PKT'
	BYTE version[2]; // 0x00, 0x03
	BYTE snifferID; 
	DWORD build; 
	BYTE language[4]; // язык клиента: 'ruRU', 'enGB' и т.д.
	BYTE sessionKey[40]; // может быть заполнено нул€ми
	DWORD optionalHeaderLength; 
};

struct PKT_CHUNK
{
	union
	{
		BYTE b[4]; // 'SMSG', 'CMSG'
		DWORD d;
	} direction;
	DWORD unixTime;
	DWORD tickCount;
	DWORD optionalDataLength;
	DWORD dataLength;
};

#pragma pack(pop)

typedef PKT_HEADER* LPPKT_HEADER;
typedef PKT_CHUNK* LPPKT_CHUNK;
