//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
#include "..\my_include\free_types_.h"
#include "DrawGridCell.h"
//------------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *N2;
	TMenuItem *N3;
	TPanel *panelStatus;
	TSplitter *Splitter2;
	TSplitter *Splitter3;
	TSplitter *Splitter4;
	TPanel *Panel9;
	TPanel *Panel10;
	TPanel *Panel11;
	TPanel *Panel12;
	TStringGrid *grd1;
	TEdit *edGrd;
	TMenuItem *menuAddSlave;
	TMenuItem *menuDelSlave;
	TMenuItem *N5;
	TPopupMenu *PopupMenu1;
	TMenuItem *N6;
	TMenuItem *N7;
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall N2Click(TObject *Sender);
	void __fastcall N3Click(TObject *Sender);
	void __fastcall grd1GetEditText(TObject *Sender, int ACol, int ARow,
          AnsiString &Value);
	void __fastcall grd1SelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall edGrdExit(TObject *Sender);
	void __fastcall edGrdKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall edGrdKeyPress(TObject *Sender, char &Key);
	void __fastcall menuAddSlaveClick(TObject *Sender);
	void __fastcall menuDelSlaveClick(TObject *Sender);
	void __fastcall N5Click(TObject *Sender);
private:	// User declarations

	// колличество панелей статуса
    enum{ STATUS_PNLS_COUNT = 4 };
    
    // правые панели
    TPanel* pnStatus_[STATUS_PNLS_COUNT];
    PVoid connectOnTransferManagerReport_;
    my::DrawGridCellManager dgcm_;
    VInt8 notAnswered_;

    bool FPorog1[256];
    bool FPorog2[256];


    void OnTransferManagerReport(unsigned context, const AnsiString& state);
    void SetStatus( const AnsiString s, unsigned idx );
    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY );
    void AddSlave(unsigned slave);

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    __fastcall ~TForm1();

    int IndexOfSlave(unsigned slave);
	void SetNotAnswered(unsigned slave);
    void SetAnswered(unsigned slave);
    VInt8 GetSlaves() const;
	uInt8 GetSlaveOfCol(int col);
    void SetSTG3Conc(unsigned slave, double conc);
    void SetSTG3Values(unsigned slave, const double*);

    void SetPorog1(unsigned slave, bool value );
    void SetPorog2(unsigned slave, bool value );

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 