//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormSearchSlaves.h"

#include "IniFileMacro.h"
#include "FooCalledOnce.hpp"
#include "MyModFN.hpp"
#include "MyProcNfo.hpp"
#include "guicon.h"
#include "AnsiStringUtils_.h"
#include "iniUtils_.h"

#include "appcfg.h"

#include "MyExeption.h"
#include "my_array_size.h"
#include "MyDynamicBitset.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSearchSlaves *FormSearchSlaves;
//---------------------------------------------------------------------------
__fastcall TFormSearchSlaves::TFormSearchSlaves(TComponent* Owner)
	: TForm(Owner)
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	MyWCout("Конструктор FormSearchDAU.\n");
	Caption = "Поиск по адрессам";

}
//---------------------------------------------------------------------------
__fastcall TFormSearchSlaves::~TFormSearchSlaves()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
    TIniFile *ini = IniForm1();
	MyWCout("Удаляем FormSearchDAU...\n");
	Ini::SaveWindowPlacement( Handle, ini, "FormSearchDAU" );
}
//---------------------------------------------------------------------------
void __fastcall TFormSearchSlaves::FormActivate(TObject *Sender)
{
	::SetWindowPos(Handle, HWND_TOPMOST, Left, Top, Width, Height, 0	);
    static bool isDone = false;
    if( isDone ) return;
    isDone = true;
    ASSERT_FUNCTION_CALLED_ONCE__;
    Ini::LoadWindowPlacement( Handle, IniForm1(), "FormSearchDAU" );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
LRESULT TFormSearchSlaves::OnSearchAddr(unsigned slave, bool wasFound)
{
	if(slave==0 || slave>255) return 0;
    Edit1->Text = ::IntToHex( (int)slave, 2);
    struct Hlpr
    {
    	explicit Hlpr(TEdit *e) : ed(e)
        {
        	bs = my::StrFToBitset( ed->Text.c_str() );
            bs.resize(256);
            bs[0] = 0;
        }
        ~Hlpr()
        {
        	ed->Text = my::BitsetToStrF(bs).c_str();
        }
        TEdit *ed;
        boost::dynamic_bitset<> bs;
    };
    // опрошенные
    Hlpr oposh(Edit2); 		oposh.bs[slave] = 1;
    // осталось
    Hlpr neoprosh(Edit3);	neoprosh.bs[slave] = 0;

    Hlpr otvet(wasFound ? Edit4 : Edit5 );	otvet.bs[slave] = 1;
    ProgressBar1->Position = ProgressBar1->Position + 1;   

    return 0;
}
//------------------------------------------------------------------------------
void TFormSearchSlaves::Initialize(const VInt8& slaves)
{
	leftSlaves_ = slaves;
}
//------------------------------------------------------------------------------
void __fastcall TFormSearchSlaves::Button1Click(TObject *Sender)
{
	HideMe();
}
//---------------------------------------------------------------------------


