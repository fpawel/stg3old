//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include "MyDynamicBitset.h"
#include "TransferManage.h"
#include "appcfg.h"
#include "FooCalledOnce.hpp"
#include "MyModFN.hpp"
#include "MyProcNfo.hpp"
#include "guicon.h"
#include "AnsiStringUtils_.h"
#include "iniUtils_.h"
#include "SGrdHlpr.h"
#include "uFormSearchSlaves.h"
#include "FreeAndNuLL.hpp"
#include "task_.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
AnsiString Norm1( const AnsiString& s )
{
	if( s.IsEmpty() ) return "...";
    return s;
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner),
    connectOnTransferManagerReport_
    ( TransferManager().ConnectOnReportState(OnTransferManagerReport) ),
    dgcm_(grd1)
{
	Caption = THIS_APPLICATION_TITLE;
	FormSearchSlaves = new TFormSearchSlaves(NULL);

	TIniFile *ini = IniForm1();

    // настройка панелей статуса
    pnStatus_[0] = Panel9;
    pnStatus_[1] = Panel10;
    pnStatus_[2] = Panel11;
    pnStatus_[3] = Panel12;

    // ширина панелей статуса
    for ( unsigned i=0; i<STATUS_PNLS_COUNT; ++i )
    {
        pnStatus_[i]->Width = 50;
        READ_INI_INT_VAL_("STATUS_PANEL", IntToStr(i), pnStatus_[i]->Width, 50, 2000 );
    }
    edGrd->Parent = grd1;
    dgcm_.SetAfterSetCanvasFoo( AfterSetCanvasGrd );

    grd1->ColCount = 1;
    grd1->RowCount = 2+STG3::PRMS_COUNT;
    grd1->Cells[0][0] = "Адресс";
    grd1->Cells[0][1] = "Концентрация";
    for( int i=0; i<STG3::PRMS_COUNT; ++i )
    	grd1->Cells[0][2+i] = STG3::PRM_NAME[i];

    // загружаем табличку
    ini = IniDevices();
    boost::shared_ptr<TStringList> sekts( new TStringList );
    ini->ReadSections( sekts.get() );
    VInt8 slaves;
    for( int i=0; i<sekts->Count; ++i )
    {
    	const MyInt getSlave = MyStrToInt( sekts->Strings[i] );
        if( getSlave.second && getSlave.first>0 && getSlave.first<256 )
        	slaves.push_back( getSlave.first );
    }

    grd1->ColCount = slaves.size()+1;
    for( unsigned i=0; i<slaves.size(); ++i )
    {
    	const AnsiString sekt = IntToStr( slaves.at(i) );
        grd1->Cells[i+1][0] = "0x"+IntToHex( slaves.at(i), 2 );
        for( int ro=1; ro<grd1->RowCount; ++ro )
        	grd1->Cells[i+1][ro] = ini->ReadString( sekt, IntToStr(ro), "" );
    }
    if( slaves.empty() ) while(grd1->ColCount==1) menuAddSlave->Click();
    grd1->FixedCols = 1;
    grd1->FixedRows = 1;

    for (int i = 0; i<256; i++ ){
        FPorog1[i] = false;
        FPorog2[i] = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormActivate(TObject *Sender)
{
	static bool isDone = false;
    if( isDone ) return;
    isDone = true;
    ASSERT_FUNCTION_CALLED_ONCE__;
    MyWCout("Form1->FormActivate.\n");
    ActiveControl = NULL;
	// перехват консоли на себя
    const HWND hCon = ::GetConsoleWindow();
    ::SetParent( hCon, Handle );
    ::ShowWindow( hCon, SW_HIDE );

    TIniFile *ini = IniForm1();
	Ini::LoadWindowPlacement( Handle, ini, "FORM1" );
    TransferManager().StartSimple();
}
//------------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
    TransferManager().StopThread();

    TIniFile *ini = IniForm1();

	MyWCout("Удаляем Form1...\n");
	Ini::SaveWindowPlacement( Handle, ini, "FORM1" );

    // ширина панелей статуса
    for ( unsigned i=0; i<STATUS_PNLS_COUNT; ++i )
        ini->WriteInteger( "STATUS_PANEL", IntToStr(i), pnStatus_[i]->Width );
        
    ::DeleteFile( (MyGetExePath()+"devices.cfg").c_str() );
    // сохраним табличку
    ini = IniDevices();

    for( int cl=1; cl<grd1->ColCount; ++cl )
    for( int ro=1; ro<grd1->RowCount; ++ro )
        ini->WriteString( IntToStr( GetSlaveOfCol(cl) ), IntToStr(ro), grd1->Cells[cl][ro] );

    FreeAndNULL(FormSearchSlaves);

}
//---------------------------------------------------------------------------
void TForm1::AddSlave(unsigned slave)
{
	const int cc = grd1->ColCount;
    grd1->ColCount = cc + 1;
    grd1->Cells[cc][0] = "0x"+IntToHex( (int)slave, 2);
    grd1->FixedCols = 1;
}
//---------------------------------------------------------------------------
VInt8 TForm1::GetSlaves() const
{
	const int cc = grd1->ColCount;
	VInt8 ret(cc-1);
    for( int cl=1; cl<cc; ++cl )
    {
    	ret[cl-1] = StrToInt(grd1->Cells[cl][0] );
    }
    return ret;

}
//---------------------------------------------------------------------------
uInt8 TForm1::GetSlaveOfCol(int col)
{
	assert(col>0 && col<grd1->ColCount);
	//AnsiString s = grd1->Cells[col][0];
    //s = s.SubString(2,2);
    char *endptr;
    const unsigned ret = ::strtol(grd1->Cells[col][0].c_str(), &endptr, 16);
    return ret;
}
//---------------------------------------------------------------------------
int TForm1::IndexOfSlave(unsigned slave)
{
	const VInt8 slaves = GetSlaves();
    VInt8::const_iterator i = std::find( slaves.begin(), slaves.end(), slave );
   	return i==slaves.end() ? -1 : i-slaves.begin();
}
//---------------------------------------------------------------------------
void TForm1::SetNotAnswered(unsigned slave)
{
	const int idx = IndexOfSlave(slave);
    if(idx==-1) return;
    if( std::find( notAnswered_.begin(), notAnswered_.end(), slave )==notAnswered_.end() )
    {
    	notAnswered_.push_back( slave );
		grd1->Cells[idx+1][0] = grd1->Cells[idx+1][0];
    }
    return;
}
//---------------------------------------------------------------------------
void TForm1::SetAnswered(unsigned slave)
{
	VInt8::iterator fnd = std::find( notAnswered_.begin(), notAnswered_.end(), slave );
    if( fnd!=notAnswered_.end() )
		notAnswered_.erase( fnd );
    int idx = IndexOfSlave(slave);
    if(idx==-1)
    {
    	// добавить адресс
        AddSlave(slave);
        idx = grd1->ColCount-2;
    }
    grd1->Cells[idx+1][0] = grd1->Cells[idx+1][0];
    return;
}

void TForm1::SetPorog1(unsigned slave, bool value )
{
    int idx = IndexOfSlave(slave);
    if(idx==-1)
    {
    	AddSlave(slave);
        idx = grd1->ColCount-2;
    }
    FPorog1[slave] = value;
    grd1->Cells[idx+1][4] = grd1->Cells[idx+1][4];
}
void TForm1::SetPorog2(unsigned slave, bool value )
{
    int idx = IndexOfSlave(slave);
    if(idx==-1)
    {
    	AddSlave(slave);
        idx = grd1->ColCount-2;
    }
    FPorog2[slave] = value;
    grd1->Cells[idx+1][5] = grd1->Cells[idx+1][5];
}

//---------------------------------------------------------------------------
void TForm1::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY )
{
	TCanvas &cnv = *grd->Canvas;

    if(col==0 || row==0)
    {
        cnv.Brush->Color = grd->FixedColor;
        cnv.Font->Color = clNavy;
    }

    if(col==0 ) // ВЫРАВНИВАНИЕ по правому краю
    {
    	const int txtW = cnv.TextWidth( grd->Cells[col][row] );
        txtX = rect.right - 3 - txtW;
    } else
    {
    	const uInt8 slave = GetSlaveOfCol(col);
        if( row==0&& std::find( notAnswered_.begin(), notAnswered_.end(), slave )!=notAnswered_.end() )
        {
        	cnv.Brush->Color = clGray;
            cnv.Font->Color = clWhite;
            cnv.Font->Style = TFontStyles() << fsBold;
        }
    }



    if(col>0 && row==1)
    {
    	cnv.Brush->Color = (TColor)0x00FFFBF0;
        cnv.Font->Style = TFontStyles() << fsBold;
    }

    if ( drawState & mygdSelected )
    {
    	cnv.Brush->Color = clInfoBk;
        cnv.Font->Color = clNavy;
    }

    if (col > 0) {
        uInt8 slave = GetSlaveOfCol(col);
        if ( row == 4 && FPorog1[slave] || row == 5 && FPorog2[slave]) {
            cnv.Brush->Color = clRed;
            cnv.Font->Color = clWhite;
        }
    }



}
//---------------------------------------------------------------------------
void TForm1::OnTransferManagerReport(unsigned context, const AnsiString& state)
{
	if ( !(context<RPT_TRNSFR_CNTXT::COUNT) ) return;

    const bool
        isStartThreadMsg    = context==RPT_TRNSFR_CNTXT::START_THREAD,
        isStopThread        = context==RPT_TRNSFR_CNTXT::STOP_THREAD;

    if( isStartThreadMsg || isStopThread )
    {
        SetStatus(state,0);
        for( unsigned i=1; i<STATUS_PNLS_COUNT; ++i ) SetStatus("",i);
        return;
    }

    AnsiString s = state;
    if( context==RPT_TRNSFR_CNTXT::END )
        s = "Ok";
    if( context==RPT_TRNSFR_CNTXT::ERR )
        context=RPT_TRNSFR_CNTXT::END;
    SetStatus( s, context );
}
//------------------------------------------------------------------------------
void TForm1::SetStatus( const AnsiString s, unsigned idx )
{
    assert( idx<STATUS_PNLS_COUNT );
    pnStatus_[idx]->Caption = s;
    pnStatus_[idx]->Hint = pnStatus_[idx]->Caption;
}
//------------------------------------------------------------------------------
void TForm1::SetSTG3Conc(unsigned slave, double conc)
{
	SetAnswered(slave);
    const int idx = IndexOfSlave(slave);
    if(idx==-1) return;
    grd1->Cells[1+idx][1] = FormatFloat( "#0.0#", conc );
}
//------------------------------------------------------------------------------
void TForm1::SetSTG3Values(unsigned slave, const double* pVals)
{
	SetAnswered(slave);
    const int idx = IndexOfSlave(slave);
    if(idx==-1) return;
    for( unsigned prmIdx = 0; prmIdx<STG3::PRMS_COUNT; ++prmIdx )
    	grd1->Cells[1+idx][2+prmIdx] = FormatFloat( "#0.0#", pVals[prmIdx] );
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void __fastcall TForm1::N2Click(TObject *Sender)
{
	GUIConsole::Instance().Create();
    HWND hCon = ::GetConsoleWindow();
   	::ShowWindow(hCon, ::IsWindowVisible(hCon)==FALSE ? SW_SHOWNORMAL : SW_HIDE );	
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N3Click(TObject *Sender)
{
	TransferManager().SetupDialog();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::grd1GetEditText(TObject *Sender, int ACol,
      int ARow, AnsiString &Value)
{
	TRect r = grd1->CellRect(ACol,ARow) ;
    r.top +=  1;
    r.left +=  1;
    edGrd->BoundsRect = r;
    edGrd->Text = Value;
    edGrd->Visible = 1;
    ActiveControl = edGrd;	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::grd1SelectCell(TObject *Sender, int ACol, int ARow,
      bool &CanSelect)
{
	// сделать таблицу редактируемой или нет
	my::Grd::SetEditing(grd1, ARow>1);	
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edGrdExit(TObject *Sender)
{
	grd1->OnGetEditText = NULL;
	struct HideEdGrdSmmry{	~HideEdGrdSmmry()
    {
    	Form1->edGrd->Visible = false;
        Form1->edGrd->Visible = false;
    	Form1->grd1->EditorMode = 0;
    	Form1->grd1->OnGetEditText = Form1->grd1GetEditText;
    } };
    const HideEdGrdSmmry hideEdGrdSmmry;
    grd1->OnGetEditText = NULL;
	const unsigned col = grd1->Col, row = grd1->Row;
    const AnsiString s = edGrd->Text;

	#pragma warn -8012
    const MyDouble getVal = MyStrToD(s);
    if(!getVal.second) return;
    grd1->Cells[col][row] = s;

    const unsigned paramIndex = row-2;

    if(paramIndex<STG3::PRMS_COUNT)
    {
    	TransferManager().StopThread();
    	TransferManager().AddTask( new HandleSTG3WriteTask( GetSlaveOfCol(col),
        	paramIndex, getVal.first ) );
    	TransferManager().StartSimple();
    }

    #pragma warn +8012
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edGrdKeyDown(TObject *Sender, WORD &key,
      TShiftState Shift)
{
	if( key == VK_RETURN ) key = VK_DOWN;
	if( key == VK_UP || key == VK_DOWN )
    {
    	ActiveControl = grd1;
        keybd_event(key,0,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
    	keybd_event(key,0,KEYEVENTF_EXTENDEDKEY,0);
    	keybd_event(key,0,KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
        key = 0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edGrdKeyPress(TObject *Sender, char &Key)
{
	if ( Key == '.' || Key == ',') Key = DecimalSeparator;	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuAddSlaveClick(TObject *Sender)
{
	const VInt8 slaves = GetSlaves();
    uInt8 slave = 1;
    for( ; slave<0xFFul; ++slave )
    	if( std::find( slaves.begin(), slaves.end(), slave )==slaves.end() )
        	break;
	go_there:
	AnsiString sSlave = IntToStr(slave);
	if( !InputQuery("Добавить датчик", "Введите сетевой адресс от 1 до 255", sSlave) ) return;

    const MyInt userInput = MyStrToInt( sSlave );
    slave = userInput.first;
    const bool
    	goodInput = userInput.second,
        inCorrectLimits = userInput.first>=1 && userInput.first<=255,
        uniqNum = std::find( slaves.begin(), slaves.end(), slave )==slaves.end();
    if ( !( goodInput && inCorrectLimits && uniqNum ) )

    {
    	ShowMessage("Некорректный сетевой адресс: \""+sSlave+"\"!" );
        goto go_there;
    }


    AddSlave(userInput.first);
    if( TransferManager().IsThreadStarted() )
    	TransferManager().StartSimple();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuDelSlaveClick(TObject *Sender)
{
	const int
    	//cc = grd1->ColCount,
        cl0 = grd1->Selection.Left, cl1 = grd1->Selection.Right ;
    if( cl1-cl0 + 1 == grd1->ColCount-1 )
    {
    	::MessageBox(Handle, "Нельзя очистить список устройств.",
        THIS_APPLICATION_TITLE,	MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_OK  );
    	return;
    }

    const VInt8 slaves = GetSlaves();
    AnsiString sSlavestoDel;
    for( int i=cl0; i<cl1+1; ++i ) sSlavestoDel = sSlavestoDel + grd1->Cells[i][0]+" ";

    if( MessageDlg( "Исключить следующие адресса датчиков: "+sSlavestoDel+"?",
    	mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0)!=mrYes )
    	return;
    my::Grd::DeleteCol(grd1, cl0, cl1-cl0+1);
    TransferManager().StartSimple();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N5Click(TObject *Sender)
{
	static AnsiString sAddrs = "1-255";
    if( !InputQuery("Поиск по адрессам", "Введите диапазон адрессов", sAddrs) ) return;
    const std::string s( sAddrs.c_str() );
    boost::dynamic_bitset<> addrs = my::StrFToBitset( s );
    addrs.resize(256);
    addrs[0] = 0;
    VInt8 slaves;
    for( int i=255; i>0; --i ) if (addrs[i]) slaves.push_back( i );
    if( slaves.empty() )
    {
    	ShowUserInputError("диапазон адресов", sAddrs);
        return;
    }
    TransferManager().StopThread();
    FormSearchSlaves->Initialize( slaves );
    FormSearchSlaves->ProgressBar1->Max = slaves.size();
    FormSearchSlaves->ProgressBar1->Position = 0;
    FormSearchSlaves->Edit1->Text = "";
    FormSearchSlaves->Edit2->Text = "";
    FormSearchSlaves->Edit3->Text = sAddrs;
    FormSearchSlaves->Edit4->Text = "";
    FormSearchSlaves->Edit5->Text = "";
    FormSearchSlaves->Edit1->Text = my::BitsetToStrF(addrs).c_str();

    // настроить поток опроса
    TransferManager().StartSearchSlaves();
	FormSearchSlaves->ShowModal();
    TransferManager().StopThread();

    const AnsiString msg = MYSPRINTF_(
        "Поиск по адресам завершён.\n"
        "Опрошено\t-\t%s.\n"
        "Ответили\t-\t%s.\n"
        "Не ответили\t-\t%s.\n",
        Norm1( FormSearchSlaves->Edit2->Text ),
        Norm1( FormSearchSlaves->Edit4->Text ),
        Norm1( FormSearchSlaves->Edit5->Text ) );

    ShowMessage(msg);

    TransferManager().StartSimple();
}
//---------------------------------------------------------------------------

