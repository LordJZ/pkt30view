#include "stdafx.h"
#include "filemap.h"

CFileMap::CFileMap()
	: hFile(INVALID_HANDLE_VALUE),
	  hMap(0),
	  lpMapView(0),
	  dwSize(0)
{

}

CFileMap::CFileMap(LPCTSTR path, bool write)
	: hFile(INVALID_HANDLE_VALUE),
	  hMap(0),
	  lpMapView(0),
	  dwSize(0)
{
	Open(path, write);
}

CFileMap::~CFileMap()
{
	Close();
}

BYTE* CFileMap::Base()
{
	return reinterpret_cast<BYTE*>(lpMapView);
}

DWORD CFileMap::Size()
{
	return dwSize;
}

void CFileMap::Close()
{
	if (hMap)
		CloseHandle(hMap);
	hMap = 0;
	if (lpMapView)
		UnmapViewOfFile(lpMapView);
	lpMapView = 0;
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;
	dwSize = 0;
}

bool CFileMap::OpenInternal(LPCTSTR path, DWORD dwAccess, DWORD dwCreation, DWORD flProtect, DWORD dwPageAccess, DWORD size)
{
	if (hFile != INVALID_HANDLE_VALUE)
		Close();
	hFile = CreateFile(path, dwAccess, FILE_SHARE_READ, 0, dwCreation, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	if (!size)
		size = GetFileSize(hFile, 0);
	hMap = CreateFileMapping(hFile, 0, flProtect, 0, size, 0);
	if (!hMap)
	{
		Close();
		return false;
	}

	lpMapView = MapViewOfFile(hMap, dwPageAccess, 0, 0, size);
	if (!lpMapView)
	{
		Close();
		return false;
	}
	dwSize = size;
	return true;
}

bool CFileMap::Create(LPCTSTR path, DWORD size)
{
	return OpenInternal(path, GENERIC_READ | GENERIC_WRITE, CREATE_ALWAYS, PAGE_READWRITE, FILE_MAP_WRITE, size);
}

bool CFileMap::Open(LPCTSTR path, bool write)
{
	if (write)
		return OpenInternal(path, GENERIC_READ|GENERIC_WRITE, OPEN_EXISTING, PAGE_READWRITE, FILE_MAP_READ|FILE_MAP_WRITE);
	else
		return OpenInternal(path, GENERIC_READ, OPEN_EXISTING, PAGE_READONLY, FILE_MAP_READ);
}

