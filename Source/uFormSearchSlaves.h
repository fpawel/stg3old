//---------------------------------------------------------------------------

#ifndef uFormSearchSlavesH
#define uFormSearchSlavesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------


//------------------------------------------------------------------------------
//std
//------------------------------------------------------------------------------
//vcl
//------------------------------------------------------------------------------
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
//my
#include "free_types_.h"
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
class TFormSearchSlaves : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnSearchDevice;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TProgressBar *ProgressBar1;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TButton *Button1;
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
	VInt8 leftSlaves_;
public:		// User declarations
	__fastcall TFormSearchSlaves(TComponent* Owner);
    __fastcall ~TFormSearchSlaves();

    void Initialize(const VInt8& leftSlaves);

    const VInt8 GetLeftSlaves() const { return leftSlaves_; }

    LRESULT OnSearchAddr(unsigned slave, bool wasFound);
    LRESULT HideMe()
    {
    	ModalResult = mrOk;
        return 0;
    }

};
//---------------------------------------------------------------------------
extern PACKAGE TFormSearchSlaves *FormSearchSlaves;
//---------------------------------------------------------------------------
#endif
