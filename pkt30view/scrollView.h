#pragma once
#include <Windows.h>
#include <string>
#include <deque>
#include <atlwin.h>
#include "wtl\atlscrl.h"
#include "wtl\atlmisc.h"
#include "wtl\atlgdi.h"

class CScrollView : public CScrollWindowImpl<CScrollView>
{
	std::deque<std::string> m_strings;
	SIZE m_size;
	CFont m_font;
	int m_nTextHeight;
public:
	DECLARE_WND_CLASS(NULL)

	void Insert(std::string str)
	{
		m_strings.push_back(str);
		m_size.cy = m_nTextHeight * m_strings.size();
		SetScrollSize(m_size);
		Invalidate();
	}
	void Clear()
	{
		m_strings.clear();
		m_size.cx = 1;
		m_size.cy = 1;
		SetScrollSize(m_size);
		Invalidate();
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	void DoPaint(CDCHandle dc)
	{
		dc.SelectFont(m_font);
		TEXTMETRIC tm;
		dc.GetTextMetrics(&tm);
		m_nTextHeight = tm.tmHeight;

		if(!m_strings.empty())
		{
			CRect crect;
			SIZE size;
			GetClientRect(crect);
			CRect rect(0, 0, crect.right, tm.tmHeight);
			for (int i = 0; i < int(m_strings.size()); i++)
			{
				dc.GetTextExtent(m_strings[i].c_str(), m_strings[i].length(), &size);
				if (size.cx > m_size.cx)
				{
					m_size.cx = size.cx;
					SetScrollSize(m_size);
				}
				rect.right = size.cx;
				dc.DrawText(m_strings[i].c_str(), -1, rect, DT_SINGLELINE | DT_LEFT);
				rect.top += tm.tmHeight;
				rect.bottom += tm.tmHeight;
			}
		}
	}

	BEGIN_MSG_MAP(CScrollView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		CHAIN_MSG_MAP(CScrollWindowImpl<CScrollView>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_font.CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Courier New"));
		m_size.cx = 1;
		m_size.cy = 1;
		SetScrollSize(m_size);
		return 0;
	}
};
