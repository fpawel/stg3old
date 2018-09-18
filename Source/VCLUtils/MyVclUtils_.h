//---------------------------------------------------------------------------
#ifndef MyVclUtils_H
#define MyVclUtils_H
//---------------------------------------------------------------------------
#include <IniFiles.hpp>
#include <Controls.hpp>



void IniSaveControlPlacement(TCustomIniFile* ini, TControl* ctrl, const AnsiString& sekt);
void IniLoadControlPlacement(TCustomIniFile* ini, TControl* ctrl, const AnsiString& sekt);
TWinControl* GetVCLControlAtPos( TWinControl* ctrl, const TPoint &mousePos );
bool IsChildControlOf( TWinControl* ctrl, TWinControl* parCtrl );  

typedef void (__closure *MyMouseWheelUpDownEvent)(TObject*, TShiftState,
    const TPoint &mousePos, bool &Handled, bool up);

void MySelectDirectory();

void MyCopyTextToClipboard( const WideString& pText  );     

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
