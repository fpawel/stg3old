//---------------------------------------------------------------------------
#include <algorithm>
#include <cassert>

//---------------------------------------------------------------------------

#pragma hdrstop

#include "MyVclUtils_.h"

#pragma package(smart_init)

#define SAVE_(n) ini->WriteInteger(sekt,""#n,ctrl->##n)
void IniSaveControlPlacement(TCustomIniFile* ini, TControl* ctrl, const AnsiString& sekt)
{
    SAVE_(Left);
    SAVE_(Top);
    SAVE_(Width);
    SAVE_(Height);
    SAVE_(Visible);
}
#undef SAVE_(n)


#define LOAD_(n) ctrl->##n = ini->ReadInteger(sekt,""#n,ctrl->##n)
void IniLoadControlPlacement(TCustomIniFile* ini, TControl* ctrl, const AnsiString& sekt)
{
    LOAD_(Visible);
    LOAD_(Left);
    LOAD_(Top);
    LOAD_(Width);
    LOAD_(Height); 
}
#undef LOAD_(n)

TWinControl* GetVCLControlAtPos( TWinControl* ctrl, const TPoint &mousePos )
{
    TPoint pos = ctrl->ScreenToClient(mousePos);
    TWinControl
    *cldCtrl = dynamic_cast<TWinControl*>( ctrl->ControlAtPos(pos, false, true) ),
    *ret = NULL;
	for( ; cldCtrl!=NULL; pos = cldCtrl->ScreenToClient(mousePos),
        cldCtrl = dynamic_cast<TWinControl*>( cldCtrl->ControlAtPos(pos, false, true) ) )
    	ret = cldCtrl;
    return ret;
}
//------------------------------------------------------------------------------
bool IsChildControlOf( TWinControl* ctrl, TWinControl* parCtrl )
{
	for( ; ctrl!=NULL; ctrl = ctrl->Parent )
    	if(ctrl==parCtrl) return true;
    return false;

}

void MyCopyTextToClipboard( const WideString& wstr  )
{
    const unsigned
    	strLen = wstr.Length(),
        buffSize = (strLen+1)*sizeof(wchar_t);

        HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE,  buffSize );

        wchar_t *pOut = (wchar_t*) ::GlobalLock(hMem), *pIn = wstr.c_bstr();
        std::copy(pIn, pIn + strLen, pOut);
        pOut[strLen] = L'\0';
        
        ::GlobalUnlock(hMem);
        ::OpenClipboard( NULL );
        ::EmptyClipboard();
        ::SetClipboardData( CF_UNICODETEXT, hMem);
        
        ::CloseClipboard();
}
