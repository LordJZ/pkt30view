#pragma once
class CPacket
{
public:
	CPacket(void);
	~CPacket(void);
private:
	LPBYTE m_begin;
	LPBYTE m_ptr;
	int m_len;
	LPTSTR m_buf;
public:
	void Init(LPBYTE data, int len);
	LPCTSTR ReadString(int len = 0);
};

