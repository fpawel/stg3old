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
    if( rxdLen==0 ) THROW_TRANSFER_EXCEPTION_( NoAnswer, "прибор не отвечает");
    if( rxdLen<4 ) THROW_TRANSFER_EXCEPTION_( AnswerLengthMismatch, "длина ответа меньше 4!");

    if( b[0]!=devAddr ) THROW_TRANSFER_EXCEPTION_( AnswerAdressMismatch, "несовпадение адрессов");


    const bool
        cmdCodeMatch = ( b[1]==commandCode ),
        modbusError = ( b[1]==( commandCode | 0x80 ) );

    if( !cmdCodeMatch && !modbusError )
        THROW_TRANSFER_EXCEPTION_( AnswerCommandCodeMismatch, "несовпадение кодов комманд");

    if( GetCRC16( b, e )!=0 ) THROW_TRANSFER_EXCEPTION_( BadCRC16, "ненулевой хэш");
    assert( IsModbusStr(b, e, devAddr, commandCode ) );
}
//------------------------------------------------------------------------------

// вывести массив в cout
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
    // конфигурация RS-232
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

    // значения задержек
    timeOut_ = StrToInt(dlg->edTimeOut->Text);
    tmWriteDelay_ = StrToInt(dlg->edWriteDelay->Text);
    silentTime_ = StrToInt(dlg->edSilentTime->Text);
}
//------------------------------------------------------------------------------
// Передать txd, считать данные приёмника в rxd, вернуть длительность трансфера
unsigned RS232ModbusT::PerformTransfer
(
	unsigned char modbusDeviceAddress,
	unsigned char commandCode,			// код комманды
	ByteBuffPtr sendBegin,		// ссылка на передаваемые данные
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,			// ссылка на принятые данные
    ByteBuffPtr& reciveEnd
)
{
    if( checkCancelTransfer_!=NULL && checkCancelTransfer_() ) return 0;
    // выполнить запланированную задержку
    PerformDelay( checkCancelTransfer_, tmWriteDelay_ );

	const int sendStrLen = sendEnd - sendBegin;
    assert(sendStrLen<10000);

	// подготовить посылку
    VInt8 txd( sendEnd - sendBegin + 4 );
    // формировать строку с адрессом 0x11 и контрольной суммой CRC16
    txd[0] = modbusDeviceAddress;
    txd[1] = commandCode;
    std::copy( sendBegin, sendEnd, txd.begin()+2 );
    const short unsigned int crc16 = GetCRC16 ( txd.begin(), txd.end()-2 );
    *(txd.end()-2) = crc16 >> 8;
    *(txd.end()-1) = crc16;
    
    assert( IsModbusStr(txd.begin(), txd.end(), modbusDeviceAddress, commandCode) );

    // вывести txd в cout
    LogOut( "TXD: "+ PrintVInt8( txd )+"\n" );

    // приёмопередача
    TryPerformTransfer(txd, rxd_);

    if( checkCancelTransfer_() || txd[0]==0 ) return 0;

    // проверка корректности rxd_ - возбуждаются исключения
    CheckModbusStr(rxd_.begin(), rxd_.end(), modbusDeviceAddress, commandCode);

    // формировать результат
    reciveBegin = rxd_.begin()+2;
    reciveEnd   = rxd_.end()-2;


    if( rxd_.size()==5 && ( rxd_[1] == (commandCode | 0x80) ) )
        return rxd_[2];

    return 0;
}
//------------------------------------------------------------------------------
void RS232ModbusT::TryPerformTransfer(const VInt8& txd, VInt8& rxd_)
{
    // открыть канал связи с текущими настройками
    OpenChanal();
    // очистка буфферов
    comm_.Purge();
    rxd_.clear();

    // отправить запрос
    const unsigned txdSz = txd.size();
    void *pTxdBuf = reinterpret_cast<void*>( const_cast<unsigned char*>(&txd.at(0)) );
    const int actualWritenCount = comm_.Write( pTxdBuf, txdSz );

    if( (unsigned)actualWritenCount!=txdSz )
    {
        const AnsiString errMsg = AnsiString().sprintf(
            "%s: ошибка считывания: %d байт из %d!",
            comm_.GetPortName(), actualWritenCount, txdSz );
        throw MyExcptionFactory::Create<MyException>(GET_FLINE_, errMsg.c_str() );
    }

    // не ждём ответа на широковещательные сообщения
    if( txd[0]==0 ) return;

    // цикл считывания из RxD
    // запомнить момент начала считывания из RxD
    unsigned tmStart = GetTickCount();

    // цикл считывания из RxD
    while
    (
        !checkCancelTransfer_() &&
        //!IsModbusStr(rxd_.begin(), rxd_.end(), txd[0], txd[1] ) &&
        (GetTickCount() - tmStart < timeOut_)
    )
    {
		// определим количество байтов в буффере порта
        const int rxdSize = comm_.GetRxDSize();
        // если RxD пуст, продолжаем мониторинг
        if( rxdSize==0 )
        {
            ::Sleep(1);
            continue;
        }
        //cout << "Has got rxd " << rxdSize << "\n";

        // в приёмнике появились данные. Считываем
        const unsigned oldReadBuffSize = rxd_.size();
        rxd_.resize(oldReadBuffSize + rxdSize);
        comm_.Read( reinterpret_cast<void*>( &rxd_.at(oldReadBuffSize) ), rxdSize );

        // ждём ещё чутка и проверяем длину RxD. Если не 0, продолжаем считывание
        // иначе считаем что посылка завершена
        ::Sleep(silentTime_);
        if( comm_.GetRxDSize()==0 ) break;
        tmStart = ::GetTickCount();


    }
    LogOut( MYSPRINTF_("%d мсек\n", GetTickCount() - tmStart)  );
    if( GetTickCount() - tmStart >= timeOut_ )
        LogOut( "!!ТАЙМАУТ!!\n" );

    if( !rxd_.empty() )
    	LogOut( "RXD: "+ PrintVInt8( rxd_ )+"\n" );
    else
    	LogOut( "Нет ответа!\n" );
}
//------------------------------------------------------------------------------
// открыть канал связи с текущими настройками
void RS232ModbusT::OpenChanal()
{
	if( comm_.IsConnected() ) return;
    const AnsiString
    	dscr = comm_.GetPortName(),
    	msg = MYSPRINTF_("Соединение с \"%s\"....\n", dscr );
    reportState_( RPT_TRNSFR_CNTXT::ITFC, msg  );
    my::SynchronizeWCout( msg );

    try
    {
    	comm_.Open();
        reportState_( RPT_TRNSFR_CNTXT::ITFC, dscr );
    	my::SynchronizeWCout( "соединение установлено.\n" );
    }
    catch(...)
    {
    	// подождать секунду, чтобы постоянно не дёргать не рабочий порт
    	Sleep(1000);
        //throw;
        MyExcptionFactory::Create<MyException>( GET_FLINE_,
            "Ошибка при инициализации "+dscr, MY_RETHROW_ );
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






