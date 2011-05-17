#include "StdAfx.h"
#include "Packet.h"


CPacket::CPacket(void)
	: m_begin(NULL)
	, m_ptr(NULL)
	, m_len(0)
	, m_buf(NULL)
{
}


CPacket::~CPacket(void)
{
	if (m_buf)
	{
		delete m_buf;
	}
}


void CPacket::Init(LPBYTE data, int len)
{
	m_begin = data;
	m_ptr = data;
	m_len = len;
}


LPCTSTR CPacket::ReadString(int len)
{
	int bufsize;
	if (m_buf)
	{
		delete m_buf;
		m_buf = NULL;
	}
	
	if (len)
		bufsize = len + 1;
	else
		bufsize = m_len - (m_ptr - m_begin) + 1;

	m_buf = new TCHAR[bufsize];
	memset(m_buf, 0, bufsize);

	TCHAR c;
	int i = 0;
	do
	{
		c = *m_ptr;
		m_buf[i] = c;
		i++;
		m_ptr++;
	} while ((c) && (i < bufsize) && (m_ptr < m_begin + m_len));
	
	return m_buf;
}
