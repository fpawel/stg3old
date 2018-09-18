//------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//------------------------------------------------------------------------------
#include "uFrmWait.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrmWait *FrmWait;
//------------------------------------------------------------------------------
__fastcall TFrmWait::TFrmWait(TComponent* Owner, bool withProgressBar ) : TForm(Owner)
{
    FixPosition();
    if( !withProgressBar ) return;
    TProgressBar& pb = *pb1;
    pb.Min = 0;
    pb.Position = 0;
    pb.Visible = 1;
}
//------------------------------------------------------------------------------
__fastcall TFrmWait::~TFrmWait()
{
	//
}
//------------------------------------------------------------------------------
void __fastcall TFrmWait::FormCreate(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TFrmWait::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caNone;
}
//------------------------------------------------------------------------------
void __fastcall TFrmWait::FormActivate(TObject *Sender)
{
	SetWindowPos(Handle, HWND_TOPMOST, Left, Top, Width, Height, 0	);
}
//------------------------------------------------------------------------------
void TFrmWait::FixPosition()
{
    TFont *fnt = new TFont;
    fnt->Assign( Canvas->Font );
    Canvas->Font->Assign( Panel2->Font );
    Width = 100 + Canvas->TextWidth( Panel2->Caption );
    Canvas->Font->Assign( fnt );

}

