//---------------------------------------------------------------------------
#ifndef MyWindowH
#define MyWindowH
//---------------------------------------------------------------------------

//std
#include <map>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// windows
#include <basetsd.h>

typedef LRESULT (__closure *HandleWindowMessageMethod )( HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParamm, bool &isHandled );

// �������� � ������� ���������� ����� �������
void MyChainWindowProc( HWND hWnd, HandleWindowMessageMethod method );

// �������� � ������� ���������� ����� �������
HWND MyAllocateWindow( const char* wndClassName, const char* wndName, HandleWindowMessageMethod method );

int MyMessageBox(HWND hWnd,	LPCTSTR lpText,	LPCTSTR lpCaption,	UINT uType, char emptyWndName[] = ""   );






//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
