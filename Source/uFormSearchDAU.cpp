//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormSearchDAU.h"


#include "uFormDAUList.h"
#include "DrawGridCell.h"
#include "SGrdHlpr.h"

#include "IniFileMacro.h"
#include "FooCalledOnce.hpp"
#include "MyModFN.hpp"
#include "MyProcNfo.hpp"
#include "guicon.h"
#include "AnsiStringUtils_.h"
#include "iniUtils_.h"

// Интерфейс управления системой контроля потребления электроэнергии.
#include "CcasImpl.h"
#include "CcasDecl.h"
#include "DataAcquisitionUnit.h"
#include "appcfg.h"

#include "MyExeption.h"
#include "con_in.h"
#include "DrawGridCell.h"
#include "my_array_size.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSearchDAU *FormSearchDAU;
//---------------------------------------------------------------------------
__fastcall TFormSearchDAU::TFormSearchDAU(TComponent* Owner)
	: TForm(Owner)
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	MyWCout("Конструктор FormSearchDAU.\n");
	Caption = "Поиск модулей УСД по адрессам";

}
//---------------------------------------------------------------------------
__fastcall TFormSearchDAU::~TFormSearchDAU()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
    TIniFile *ini = IniForm1();
	MyWCout("Удаляем FormSearchDAU...\n");
	Ini::SaveWindowPlacement( Handle, ini, "FormSearchDAU" );
}
//---------------------------------------------------------------------------
void __fastcall TFormSearchDAU::FormActivate(TObject *Sender)
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
LRESULT TFormSearchDAU::OnSearchAddr(unsigned slave, bool wasFound)
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
void TFormSearchDAU::Initialize(const VInt8& slaves)
{
	leftSlaves_ = slaves;
}
//------------------------------------------------------------------------------
void __fastcall TFormSearchDAU::Button1Click(TObject *Sender)
{
	Ccas().StopJob();
	HideMe();
}
//---------------------------------------------------------------------------

