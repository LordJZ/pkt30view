#pragma once
#include "lua\lua.hpp"
#include "packet.h"
#include "scrollView.h"

class CLuaVM
{
public:
	CLuaVM(void);
	~CLuaVM(void);
private:
	lua_State* m_LuaState;
	static CPacket m_packet;
	static CScrollView* m_out;
private:
	static int lua_length(lua_State* L);
	static int lua_print(lua_State* L);
	static int lua_readstring(lua_State* L);
public:
	void InitModule(LPCTSTR filename);
	void CallFunc(LPCTSTR psName, LPBYTE data, int len, void* out);
};

