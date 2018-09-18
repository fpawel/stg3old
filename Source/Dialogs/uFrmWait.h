//---------------------------------------------------------------------------

#ifndef uFrmWaitH
#define uFrmWaitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//---------------------------------------------------------------------------

class TFrmWait : public TForm
{
__published:	// IDE-managed Components
    TProgressBar *pb1;
    TPanel *Panel2;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
	
public:		// User declarations
	__fastcall TFrmWait(TComponent* Owner, bool withProgressBar = false  );
    virtual __fastcall ~TFrmWait();
    void FixPosition();
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmWait *FrmWait;
//---------------------------------------------------------------------------


#endif
