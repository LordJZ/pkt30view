#include "StdAfx.h"
#include "Pkt30viewDoc.h"

extern TCHAR gWorkDir[MAX_PATH];

CPkt30viewDoc::CPkt30viewDoc(void)
	: m_StartTick(0)
{
}


CPkt30viewDoc::~CPkt30viewDoc(void)
{
}

bool CPkt30viewDoc::OpenFile(LPCTSTR psName)
{
	if (!m_FileMap.Open(psName, false))
		throw (_T("Error opening file"));

	if (!CheckHeader())
		throw (_T("Invalid header"));

	BuildIndex();
	LoadOpcodes();
	LoadParsers();

	return true;
}

bool CPkt30viewDoc::CheckHeader(void)
{
	if (!m_FileMap)
		return false;

	memcpy(&m_Header, m_FileMap.Base(), sizeof(m_Header));
	if ((m_Header.signature[0] == 'P') && (m_Header.signature[1] == 'K') && (m_Header.signature[2] == 'T'))
	{
		if ((m_Header.version[0] == 0) && (m_Header.version[1] == 3))
			return true;
	}
	return false;
}

void CPkt30viewDoc::BuildIndex(void)
{
	m_Index.clear();

	LPPKT_CHUNK pChunk = NULL;
	LPBYTE ptr = m_FileMap.Base();
	LPBYTE ptr_end = m_FileMap.Base() + m_FileMap.Size();
	
	if (m_FileMap)
	{
		// пропускаем заголовок и дополнительные данные заголовка
		ptr += (sizeof(m_Header) + m_Header.optionalHeaderLength);
		while (ptr < ptr_end)
		{
			pChunk = (LPPKT_CHUNK)ptr;
			if (m_StartTick == 0)
				m_StartTick = pChunk->tickCount;
			m_Index.push_back(ptr);
			ptr += (sizeof(PKT_CHUNK) + pChunk->dataLength + pChunk->optionalDataLength);
		}
	}
}

void CPkt30viewDoc::LoadOpcodes(void)
{
	TCHAR str[1024], filename[MAX_PATH], opcode_str[200], opcode_val[200], lua_str[200];

	m_LuaFuncNames.clear();
	m_Opcodes.clear();

	sprintf_s(filename, "%s\\%d\\opcodes.txt", gWorkDir, m_Header.build);
	std::ifstream file(filename);
	if (file)
	{
		while (file.getline(str, 1024))
		{
			memset(opcode_str, 0, 200);
			memset(opcode_val, 0, 200);
			memset(lua_str, 0, 200);
			int step = 0, index = 0;
			for (int i = 0; (i < 1024) && str[i]; i++)
			{
				if ((str[i] != 0x20) && (str[i] != 0x09))
				{
					switch(step)
					{
					case 0:
						if (str[i] == ',')
						{
							step++;
							index = 0;
						}
						else
							opcode_val[index++] = str[i];
						break;
					case 1:
						if (str[i] == ',')
						{
							step++;
							index = 0;
						}
						else
							opcode_str[index++] = str[i];
						break;
					case 2:
						lua_str[index++] = str[i];
						break;
					}
				}
			}
			
			int opcode = 0;
			if (opcode_val[0])
			{
				char *end, *start;
				int base;
				if ((opcode_val[0] == '0') && ((opcode_val[1] == 'x') || (opcode_val[1] == 'X')))
				{
					base = 16;
					start = &opcode_val[2];
				}
				else
				{
					base = 10;
					start = &opcode_val[0];
				}
				opcode = strtol(start, &end, base);
				
				if (opcode_str[0])
				{
					std::string str2(opcode_str);
					m_Opcodes.insert(std::make_pair(opcode, str2));
				}
				if (lua_str[0])
				{
					std::string str3(lua_str);
					m_LuaFuncNames.insert(std::make_pair(opcode, str3));
				}
			}
		}
	}
}

void CPkt30viewDoc::LoadParsers(void)
{
	TCHAR filename[MAX_PATH];
	sprintf_s(filename, "%s\\%d\\parsers.lua", gWorkDir, m_Header.build);
	try
	{
		m_luaVM.InitModule(filename);
	}
	catch(...)
	{
	}
}


void CPkt30viewDoc::OnItemChange(int nItem)
{
	RunLuaFunc(nItem);
}


int CPkt30viewDoc::GetItemsCount(void)
{
	return int(m_Index.size());
}


LPPKT_CHUNK CPkt30viewDoc::GetPacketChunk(int nIndex)
{
	return (LPPKT_CHUNK)m_Index[nIndex];
}


int CPkt30viewDoc::GetStartTick(void)
{
	return m_StartTick;
}


LPCTSTR CPkt30viewDoc::GetOpcodeName(int nIndex)
{
	int opcode = GetPacketOpcode(nIndex);
	if (m_Opcodes.count(opcode))
		return m_Opcodes[opcode].c_str();
	
	sprintf_s(m_strbuf, "0x%04X", opcode);
	return m_strbuf;
}


int CPkt30viewDoc::GetPacketOpcode(int nIndex)
{
	LPPKT_CHUNK chunk = GetPacketChunk(nIndex);
	return *(int *)(m_Index[nIndex] + sizeof(PKT_CHUNK) + chunk->optionalDataLength);
}


LPCTSTR CPkt30viewDoc::GetLuaFuncName(int nIndex)
{
	int opcode = GetPacketOpcode(nIndex);
	if (m_LuaFuncNames.count(opcode))
		return m_LuaFuncNames[opcode].c_str();

	return NULL;
}


void CPkt30viewDoc::RunLuaFunc(int nIndex)
{
	LPCTSTR luaFuncName = GetLuaFuncName(nIndex);
	if (luaFuncName)
	{
		m_luaVM.CallFunc(luaFuncName, GetPacketData(nIndex), GetPacketChunk(nIndex)->dataLength - sizeof(int), m_out);
 	}
}


LPBYTE CPkt30viewDoc::GetPacketData(int nIndex)
{
	LPPKT_CHUNK chunk = GetPacketChunk(nIndex);
	return (m_Index[nIndex] + sizeof(PKT_CHUNK) + chunk->optionalDataLength + sizeof(int));
}


void CPkt30viewDoc::OnRefresh(void)
{
	LoadOpcodes();
	LoadParsers();
}
