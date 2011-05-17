#include "StdAfx.h"
#include "LuaVM.h"


CLuaVM::CLuaVM(void)
	: m_LuaState(NULL)
{
}

CLuaVM::~CLuaVM(void)
{
	if (m_LuaState)
		lua_close(m_LuaState);
}

void CLuaVM::InitModule(LPCTSTR filename)
{
	if (m_LuaState)
		lua_close(m_LuaState);

	m_LuaState = lua_open();
	if (!m_LuaState)
		throw("Error opening lua state");

	static const luaL_Reg packet[] =
	{
		{"length", lua_length},
//		{"read8", lua_read8},
//		{"read16", lua_read16},
//		{"read32", lua_read32},
		{"readstring", lua_readstring},
//		{"readguid", lua_readguid},
//		{"readpackedguid", lua_readpackedguid},
//		{"readfloat", lua_readfloat},
//		{"decompress", lua_decompress},
		{0, 0}
	};	
	luaL_register(m_LuaState, "packet", packet);

	static const luaL_Reg output[]=
	{
		{"print", lua_print},
		{0, 0}
	};	
	luaL_register(m_LuaState, "output", output);

    if (luaL_loadfile(m_LuaState, filename) || lua_pcall(m_LuaState, 0, 0, 0))
	{
		TCHAR str[512];
		sprintf_s(str, "Error loading & executing file ""%s""", filename);
		throw(str);
	}
}

int CLuaVM::lua_length(lua_State* L)
{
	return 0;
}

int CLuaVM::lua_readstring(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 0)
	{
		lua_pushstring(L, m_packet.ReadString());
		return 1;
	}

	if (argc == 1)
	{
		if (lua_isnumber(L, 1))
		{
			lua_pushstring(L, m_packet.ReadString(lua_tointeger(L, 1)));
			return 1;
		}
	}

	return 0;
}

int CLuaVM::lua_print(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc)
	{
		if (lua_isstring(L, 1))
		{
			if (m_out)
				m_out->Insert(lua_tostring(L, 1));
		}
	}
	return 1;
}


void CLuaVM::CallFunc(LPCTSTR psName, LPBYTE data, int len, void* out)
{
	m_packet.Init(data, len);
	m_out = static_cast<CScrollView*>(out);

	lua_getglobal(m_LuaState, psName);
	if (lua_pcall(m_LuaState, 0, 0, 0) != 0)
	{
		throw(lua_tostring(m_LuaState, -1));
	}
}

CPacket			CLuaVM::m_packet;
CScrollView*	CLuaVM::m_out;
