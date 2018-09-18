//---------------------------------------------------------------------------
#include <windows.h>

//std
#include <sstream>

#pragma hdrstop

// loki
#include "loki\threads.h"

#include "RS232Modbus.h"
#include "CRC16.h"

#include "bcd_.h"

//my
#include "..\appcfg.h"
#include "AnsiStringUtils_.h"
#include "iniUtils_.h"
#include "CommSetsIOHelper.h"
#include "uFormComPortSetsDlg.h"



//---------------------------------------------------------------------------
using std::cout;
using std::endl;

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

#undef MY_THROW_
#undef MY_THROW_CLASS_
#define THROW_TRANSFER_EXCEPTION_(THECLASS, MSG)\
	throw MyExcptionFactory::Create<MyTransfer##THECLASS##Exception, unsigned>(devAddr, GET_FLINE_, MSG)

//------------------------------------------------------------------------------
void SetComboBox( TComboBox* cb, const AnsiString& s )
{
    cb->ItemIndex = cb->Items->IndexOf( s );
}
//------------------------------------------------------------------------------
bool IsModbusStr(const uInt8 *b, const uInt8 *e, uInt8 devAddr, uInt8 commandCode)
{
	return !(e-b<4) && b[0]==devAddr &&
    ( b[1]==commandCode || b[1]==( commandCode | 0x80 ) ) &&
    GetCRC16( b, e )==0;
}
//------------------------------------------------------------------------------
void CheckModbusStr(const uInt8 *b, const uInt8 *e, uInt8 devAddr, uInt8 commandCode)
{
	const int rxdLen = e-b;
    assert(rxdLen>=0);
    if( rxdLen==0 ) THROW_TRANSFER_EXCEPTION_( NoAnswer, "������ �� ��������");
    if( rxdLen<4 ) THROW_TRANSFER_EXCEPTION_( AnswerLengthMismatch, "����� ������ ������ 4!");

    if( b[0]!=devAddr ) THROW_TRANSFER_EXCEPTION_( AnswerAdressMismatch, "������������ ��������");


    const bool
        cmdCodeMatch = ( b[1]==commandCode ),
        modbusError = ( b[1]==( commandCode | 0x80 ) );

    if( !cmdCodeMatch && !modbusError )
        THROW_TRANSFER_EXCEPTION_( AnswerCommandCodeMismatch, "������������ ����� �������");

    if( GetCRC16( b, e )!=0 ) THROW_TRANSFER_EXCEPTION_( BadCRC16, "��������� ���");
    assert( IsModbusStr(b, e, devAddr, commandCode ) );
}
//------------------------------------------------------------------------------

// ������� ������ � cout
AnsiString PrintVInt8( const VInt8& v, int lim = -1 )
{
    const unsigned maxIdx = (lim==-1) ? v.size() : std::min(lim, v.size() );
    return MyBuffToStr( v.begin(), v.begin() + maxIdx );
}

void PerformDelay(TBoolMthd mthd, unsigned tm)
{
	const unsigned tmStart = GetTickCount();
    while( GetTickCount()-tmStart < tm && !mthd() ) ;
}
//---------------------------------------------------------------------------
RS232ModbusT::RS232ModbusT( ) :
    logLifeTime_( MakeMyLogQuote("RS232ModbusT.[members]") ),
	checkCancelTransfer_( &NoBreakTransfer ), reportState_( &NoReport ),
    comm_()
{
    LOG_FOO_LIFETIME_QUOTES;

    TIniFile *ini_ = IniComm();
    // ������������ RS-232
    comm_.SetPortName( ini_->ReadString( TRANSFERS_SEKT, "COMPORT", "COM1") );

    my::RS232::Setings sets = comm_.GetSettings();
    sets.dcb_.BaudRate = 9600;
    comm_.SetSettings(sets);

    #define GET_INI_INT_(iid, valMin, valMax, defVal) Ini::GetInteger( ini_, TRANSFERS_SEKT, iid, valMin, valMax, defVal)
    tmWriteDelay_	 	 = GET_INI_INT_( WRITE_DELAY_ID, 0, 200000, 50);
    timeOut_			 = GET_INI_INT_( TIME_OUT_ID, 100, 500000, 1000);
    silentTime_        = GET_INI_INT_( RXD_FRAME_SILENT_TIME, 5, 100000, 10);
    mustLogData_ = ini_->ReadBool( TRANSFERS_SEKT, MUST_I_LOG_COMPORT_DATA, false );
}
//------------------------------------------------------------------------------
RS232ModbusT::~RS232ModbusT()
{
    LOG_FOO_LIFETIME_QUOTES;
    TIniFile *ini = IniComm();

    ini->WriteString( TRANSFERS_SEKT, "COMPORT", comm_.GetPortName() );

    ini->WriteInteger( TRANSFERS_SEKT, TIME_OUT_ID, timeOut_ );
    ini->WriteInteger( TRANSFERS_SEKT, RXD_FRAME_SILENT_TIME, silentTime_ );
    ini->WriteInteger( TRANSFERS_SEKT, WRITE_DELAY_ID, tmWriteDelay_ );
    ini->WriteBool( TRANSFERS_SEKT, MUST_I_LOG_COMPORT_DATA, mustLogData_ );
}
//------------------------------------------------------------------------------
void RS232ModbusT::SyncronizedSleep( unsigned tm )
{
	const unsigned tmStart = ::GetTickCount();
    while( (GetTickCount()-tmStart < tm) && !checkCancelTransfer_() ) ;
}
//------------------------------------------------------------------------------
void RS232ModbusT::LogOut( const AnsiString& s)
{
	if( !mustLogData_ ) return;
    my::SynchronizeWCout( s );
}
//------------------------------------------------------------------------------
void RS232ModbusT::SetupDialog()
{
	boost::shared_ptr<TFormComPortSetsDlg> dlg( new TFormComPortSetsDlg(NULL) );
    dlg->edSilentTime->Text = silentTime_;
    dlg->edWriteDelay->Text = tmWriteDelay_;
    dlg->edTimeOut->Text = timeOut_;
    SetComboBox( dlg->cbPortName, AnsiString().sprintf("COM%d", comm_.GetPortName() ) );
    dlg->ShowModal();
    if( dlg->ModalResult!=mrOk ) return;

    // COM
    comm_.SetPortName( dlg->cbPortName->Text );

    // �������� ��������
    timeOut_ = StrToInt(dlg->edTimeOut->Text);
    tmWriteDelay_ = StrToInt(dlg->edWriteDelay->Text);
    silentTime_ = StrToInt(dlg->edSilentTime->Text);
}
//------------------------------------------------------------------------------
// �������� txd, ������� ������ �������� � rxd, ������� ������������ ���������
unsigned RS232ModbusT::PerformTransfer
(
	unsigned char modbusDeviceAddress,
	unsigned char commandCode,			// ��� ��������
	ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,			// ������ �� �������� ������
    ByteBuffPtr& reciveEnd
)
{
    if( checkCancelTransfer_!=NULL && checkCancelTransfer_() ) return 0;
    // ��������� ��������������� ��������
    PerformDelay( checkCancelTransfer_, tmWriteDelay_ );

	const int sendStrLen = sendEnd - sendBegin;
    assert(sendStrLen<10000);

	// ����������� �������
    VInt8 txd( sendEnd - sendBegin + 4 );
    // ����������� ������ � �������� 0x11 � ����������� ������ CRC16
    txd[0] = modbusDeviceAddress;
    txd[1] = commandCode;
    std::copy( sendBegin, sendEnd, txd.begin()+2 );
    const short unsigned int crc16 = GetCRC16 ( txd.begin(), txd.end()-2 );
    *(txd.end()-2) = crc16 >> 8;
    *(txd.end()-1) = crc16;
    
    assert( IsModbusStr(txd.begin(), txd.end(), modbusDeviceAddress, commandCode) );

    // ������� txd � cout
    LogOut( "TXD: "+ PrintVInt8( txd )+"\n" );

    // �������������
    TryPerformTransfer(txd, rxd_);

    if( checkCancelTransfer_() || txd[0]==0 ) return 0;

    // �������� ������������ rxd_ - ������������ ����������
    CheckModbusStr(rxd_.begin(), rxd_.end(), modbusDeviceAddress, commandCode);

    // ����������� ���������
    reciveBegin = rxd_.begin()+2;
    reciveEnd   = rxd_.end()-2;


    if( rxd_.size()==5 && ( rxd_[1] == (commandCode | 0x80) ) )
        return rxd_[2];

    return 0;
}
//------------------------------------------------------------------------------
void RS232ModbusT::TryPerformTransfer(const VInt8& txd, VInt8& rxd_)
{
    // ������� ����� ����� � �������� �����������
    OpenChanal();
    // ������� ��������
    comm_.Purge();
    rxd_.clear();

    // ��������� ������
    const unsigned txdSz = txd.size();
    void *pTxdBuf = reinterpret_cast<void*>( const_cast<unsigned char*>(&txd.at(0)) );
    const int actualWritenCount = comm_.Write( pTxdBuf, txdSz );

    if( (unsigned)actualWritenCount!=txdSz )
    {
        const AnsiString errMsg = AnsiString().sprintf(
            "%s: ������ ����������: %d ���� �� %d!",
            comm_.GetPortName(), actualWritenCount, txdSz );
        throw MyExcptionFactory::Create<MyException>(GET_FLINE_, errMsg.c_str() );
    }

    // �� ��� ������ �� ����������������� ���������
    if( txd[0]==0 ) return;

    // ���� ���������� �� RxD
    // ��������� ������ ������ ���������� �� RxD
    unsigned tmStart = GetTickCount();

    // ���� ���������� �� RxD
    while
    (
        !checkCancelTransfer_() &&
        //!IsModbusStr(rxd_.begin(), rxd_.end(), txd[0], txd[1] ) &&
        (GetTickCount() - tmStart < timeOut_)
    )
    {
		// ��������� ���������� ������ � ������� �����
        const int rxdSize = comm_.GetRxDSize();
        // ���� RxD ����, ���������� ����������
        if( rxdSize==0 )
        {
            ::Sleep(1);
            continue;
        }
        //cout << "Has got rxd " << rxdSize << "\n";

        // � �������� ��������� ������. ���������
        const unsigned oldReadBuffSize = rxd_.size();
        rxd_.resize(oldReadBuffSize + rxdSize);
        comm_.Read( reinterpret_cast<void*>( &rxd_.at(oldReadBuffSize) ), rxdSize );

        // ��� ��� ����� � ��������� ����� RxD. ���� �� 0, ���������� ����������
        // ����� ������� ��� ������� ���������
        ::Sleep(silentTime_);
        if( comm_.GetRxDSize()==0 ) break;
        tmStart = ::GetTickCount();


    }
    LogOut( MYSPRINTF_("%d ����\n", GetTickCount() - tmStart)  );
    if( GetTickCount() - tmStart >= timeOut_ )
        LogOut( "!!�������!!\n" );

    if( !rxd_.empty() )
    	LogOut( "RXD: "+ PrintVInt8( rxd_ )+"\n" );
    else
    	LogOut( "��� ������!\n" );
}
//------------------------------------------------------------------------------
// ������� ����� ����� � �������� �����������
void RS232ModbusT::OpenChanal()
{
	if( comm_.IsConnected() ) return;
    const AnsiString
    	dscr = comm_.GetPortName(),
    	msg = MYSPRINTF_("���������� � \"%s\"....\n", dscr );
    reportState_( RPT_TRNSFR_CNTXT::ITFC, msg  );
    my::SynchronizeWCout( msg );

    try
    {
    	comm_.Open();
        reportState_( RPT_TRNSFR_CNTXT::ITFC, dscr );
    	my::SynchronizeWCout( "���������� �����������.\n" );
    }
    catch(...)
    {
    	// ��������� �������, ����� ��������� �� ������ �� ������� ����
    	Sleep(1000);
        //throw;
        MyExcptionFactory::Create<MyException>( GET_FLINE_,
            "������ ��� ������������� "+dscr, MY_RETHROW_ );
    }
    //assert( comm_.IsConnected() );
}
//------------------------------------------------------------------------------
void RS232ModbusT::CloseChanal()
{
	if( !comm_.IsConnected() ) return;
    //comm_.Close();
}
//------------------------------------------------------------------------------






