#pragma once
#include "filemap.h"
#include "pkt30.h"
#include <deque>
#include <map>
#include <string>
#include <fstream>
#include "luavm.h"

class CPkt30viewDoc
{
public:
	CPkt30viewDoc(void);
	~CPkt30viewDoc(void);
private:
	TCHAR m_strbuf[1024];
	CFileMap m_FileMap;
	PKT_HEADER m_Header;
	std::deque<LPBYTE> m_Index;
	std::map<int, std::string> m_Opcodes;
	std::map<int, std::string> m_LuaFuncNames;
	int m_StartTick;
	CLuaVM m_luaVM;
public:
	bool OpenFile(LPCTSTR psName);
	void* m_out;
private:
	bool CheckHeader(void);
	void BuildIndex(void);
	void LoadOpcodes(void);
	void LoadParsers(void);
public:
	void OnItemChange(int nItem);
	int GetItemsCount(void);
	LPPKT_CHUNK GetPacketChunk(int nIndex);
	int GetStartTick(void);
	LPCTSTR GetOpcodeName(int nIndex);
	int GetPacketOpcode(int nIndex);
	LPCTSTR GetLuaFuncName(int nIndex);
	void RunLuaFunc(int nIndex);
	LPBYTE GetPacketData(int nIndex);
	void OnRefresh(void);
};

