#pragma once

class CFileMap
{
public:
	CFileMap();
	CFileMap(LPCTSTR path, bool write = false);
	~CFileMap();

	operator bool(){ return (hFile != INVALID_HANDLE_VALUE); }
	bool    Open(LPCTSTR path, bool write);
	bool    Create(LPCTSTR path, DWORD size);
	void    Close();

	BYTE*   Base();
	DWORD   Size();
	bool    OpenInternal(LPCTSTR path, DWORD dwAccess, DWORD dwCreation, DWORD flProtect, DWORD dwPageAccess, DWORD size = 0);
private:
	HANDLE  hFile, hMap;
	LPVOID  lpMapView;
	DWORD   dwSize;
};

