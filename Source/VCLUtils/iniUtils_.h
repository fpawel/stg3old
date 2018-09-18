//---------------------------------------------------------------------------
#ifndef iniUtils_H
#define iniUtils_H
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include <inifiles.hpp>

namespace Ini
{   
    void SaveWindowPlacement( HWND hWnd, TCustomIniFile* ini, const AnsiString& sekt );
    void LoadWindowPlacement( HWND hWnd, TCustomIniFile* ini, const AnsiString& sekt );

    int GetInteger( TCustomIniFile* ini, const AnsiString& sekt,
    	const AnsiString& id, int valMin, int valMax, int defVal);

    bool GetInt(TCustomIniFile* ini, const AnsiString& sekt,
    	const AnsiString& id, int valMin, int valMax, int& result);

};

#define READ_INI_INT_(sekt, id, mmin, mmax, def) Ini::GetInteger( ini, sekt, id, mmin, mmax, def )
#define READ_INI_INT_VAL_(sekt, id, val, mmin, mmax ) val = READ_INI_INT_(sekt, id, mmin, mmax, val)



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
