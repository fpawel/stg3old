//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------

//my
#include "winRS232.h"
#include "..\my_include\my_array_size.h"
//---------------------------------------------------------------------------
#include "MyDCBHelper.h"
#include "MyExeption.h"
//#include "MyWinExcpt.hpp"
#include "CommSetsIOHelper.h"
#include "guicon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


// ���������� �� ClearCommError
class MyClearCommErrorException : public MyException
{
	friend struct MyExcptionFactory;
    static AnsiString MakeMsg(const AnsiString& msg, unsigned long int code )
    {
    	assert( my::RS232::HasMaskCriticalError(code) && "������� ��������� ���������� COM-����� ��� ������� ����� ������!" );
        	const AnsiString s = AnsiString().sprintf(
        	"������ ������-�������� ��� �����:%s:%d:%s",
        	msg, code, my::RS232::ErrorsMaskToStr( code ) );
        return s;
    }
protected:
	MyClearCommErrorException(unsigned long int code, const FileLine& loc,
    	const AnsiString& msg, PMyExcpt nest) :
        MyException(loc, MakeMsg(msg,code), nest)
    {}
public:
	virtual AnsiString ClassName() const { return "MyClearCommErrorException"; }
};
//---------------------------------------------------------------------

namespace my
{
namespace RS232
{
	template<typename T> void ClearStruct(T& obj)  { ZeroMemory( &obj, sizeof(T) ); }
	//-------------------------------------------------------------------------/
    using std::endl;
	//-------------------------------------------------------------------------
	// ������ ��������� �� ���������
	void ApplyCommSettings(HANDLE hComm)
	{
        // ���������� ������ �������� �����/������ [�� ���������]
        SetupComm( hComm, defaultIOBuffSize, defaultIOBuffSize);

        // ���������� DCB
    	DCB dcb = {0};
        SetDefaultDCB( dcb );
        SetCommState(hComm, &dcb);

        // ���������� ��������
        COMMTIMEOUTS commTimeOuts;
        GetCommTimeouts( hComm,  &commTimeOuts );
        SetDefaultCommTimeOuts(commTimeOuts);
        SetCommTimeouts( hComm,  &commTimeOuts );
	}
    //-------------------------------------------------------------------------
    void SetDefaultCommTimeOuts(COMMTIMEOUTS& commTimeOuts)
    {
        ClearStruct(commTimeOuts);
        commTimeOuts.ReadIntervalTimeout = 1000;
        commTimeOuts.ReadTotalTimeoutMultiplier = 50;
        commTimeOuts.ReadTotalTimeoutConstant = 100;
        commTimeOuts.WriteTotalTimeoutMultiplier = 100;
        commTimeOuts.WriteTotalTimeoutConstant = 1000;
    }
    //---------------------------------------------------------------------
    // �������� ���������� �����
    void* CreatePortHandle(const AnsiString& portName)
    {
    	if( !IsPortPresentedInSystem(portName) )
        {
        	const AnsiString s = AnsiString().sprintf(
            	"COM-���� c ������������� ������ \"%s\" �� ����������� � �������!",
            	portName);
            MY_THROW_( s );
        }

        AnsiString fileName =
            portName<"COM10" ? portName : "\\\\.\\" + portName;

    	void *ret =
    	CreateFile ( fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,0, OPEN_EXISTING, 0, 0 );

        if( ret==INVALID_HANDLE_VALUE )
        {
        	MY_THROW_( AnsiString().sprintf("���� %s �����!", portName) );
        }
        return ret;
    }
    //---------------------------------------------------------------------
    void SetConfig( void *hComm, const COMMCONFIG& commConfig )
    {
    	COMMCONFIG& cmCfg = const_cast<COMMCONFIG&>(commConfig);
        DWORD sz = sizeof(COMMCONFIG);
        cmCfg.dwSize = sz;
		MY_SAFE_CALL_WIN_API(SetCommConfig, ( hComm, &cmCfg,  sz ) );
    }
    //---------------------------------------------------------------------
    COMMCONFIG GetConfig( void *hComm  )
    {
    	COMMCONFIG commConfig = {0};
        DWORD sz = sizeof(commConfig);
		MY_SAFE_CALL_WIN_API(GetCommConfig,( hComm, &commConfig,  &sz ));
        return commConfig;
    }
    //---------------------------------------------------------------------
    COMMPROP GetProperties(void* hComm)
    {
    	COMMPROP ret = {0};
        MY_SAFE_CALL_WIN_API(GetCommProperties,(hComm,  &ret));
        return ret;
    }
    //---------------------------------------------------------------------
    void SetTxDRxDBuffsSizes( void* hComm, DWORD txdSize, DWORD rxdSize )
    {
    	MY_SAFE_CALL_WIN_API(SetupComm,(hComm, txdSize, rxdSize));
        const AnsiString logStr = AnsiString().sprintf("buffer size: TxD=%d RxD=%d", txdSize, rxdSize );
    }
    //---------------------------------------------------------------------
#define OPEN_PORT_IN_THIS_MEM_FOO const Port::OpenTemporaryHelper openTemporaryHelper(hComm_, portName_ )
    //---------------------------------------------------------------------
    Port::OpenTemporaryHelper::OpenTemporaryHelper(void*& hComm, const AnsiString& portName) :
    	hComm_(hComm), wasOpened_( hComm_!=INVALID_HANDLE_VALUE )
    {
    	if(!wasOpened_)
        	hComm_ = CreatePortHandle(portName);
    }
    //---------------------------------------------------------------------
    Port::OpenTemporaryHelper::~OpenTemporaryHelper()
    {
    	if(wasOpened_) return;
		CloseHandle(hComm_);
        hComm_=INVALID_HANDLE_VALUE;
    }
    //---------------------------------------------------------------------
    Port::Port() :
    	hComm_(INVALID_HANDLE_VALUE), portName_("COM1"),
        mustApplyDCB_(true), mustApplyTimeOuts_(true),
        mustApplyBuffsSizes_(true)
    {
        ApplyBuffsSizes();
        ApplyDCB();
        ApplyTimeOuts();
    }
    Port::~Port()
    {
    	Disconnect();
    }
    //---------------------------------------------------------------------
    bool Port::IsHandleCreated() const
    {
        return hComm_!=INVALID_HANDLE_VALUE;
    }

    void Port::Open()
    {
    	if( IsConnected() ) return;
        // ��� ������. ����� �������
    	try
        {
        	// ������� ����
        	hComm_ = CreatePortHandle( portName_ );
         	// �������� ������� �����
        	Purge();
            // ��������� ����������
            ApplyBuffsSizes();
            ApplyDCB();
            ApplyTimeOuts();
        }
        catch(...)
        {
        	Disconnect();
            throw;
            //MY_RETHROW_NESTED_(AnsiString().sprintf("���� ��� �������� %s!", portName_));

        }
    }
    //-------------------------------------------------------------------------
    void Port::Close()
    {
        if( !IsConnected() ) return;
        CloseHandle(hComm_);
        hComm_ = INVALID_HANDLE_VALUE;
    }
    //-------------------------------------------------------------------------
    void Port::ApplyBuffsSizes()
    {
    	if( !IsHandleCreated() )
        {
        	mustApplyBuffsSizes_ = true;
            return;
        }
        MY_SAFE_CALL_WIN_API(SetupComm,(hComm_,sets_.txdBuffSize_,sets_.rxdBuffSize_));
        mustApplyBuffsSizes_ = false;
    }
    //--------------------------------------------------------------------------
    void Port::ApplyDCB()
    {
    	if( !IsHandleCreated() )
        {
        	mustApplyDCB_ = true;
            return;
        }
        MY_SAFE_CALL_WIN_API(SetCommState,(hComm_, &sets_.dcb_ ) );
        mustApplyDCB_ = false;
    }
    //--------------------------------------------------------------------------
    void Port::ApplyTimeOuts()
    {
    	if( !IsHandleCreated() )
        {
        	mustApplyTimeOuts_ = true;
            return;
        }
        MY_SAFE_CALL_WIN_API(SetCommTimeouts, (hComm_, &sets_.timeOuts_) );
        mustApplyTimeOuts_ = false;
    }
    //---------------------------------------------------------------------
    // �������� ������� �����
    void Port::Purge()
    {
    	MY_SAFE_CALL_WIN_API(PurgeComm,( hComm_,  PURGE_TXABORT | PURGE_RXABORT
        | PURGE_TXCLEAR |PURGE_RXCLEAR ) );
    }
    //---------------------------------------------------------------------
    // �������� ���������� ������ � ��������.
    // ���� ��������� ������ �����-��������, ��������� ���������� CommException
    int Port::GetRxDSize()
    {
    	DWORD commErrors = 0;
        COMSTAT commStat = {0};
        MY_SAFE_CALL_WIN_API(ClearCommError,( hComm_, &commErrors, &commStat));
        if ( HasMaskCriticalError(commErrors) ){
            MyWCout( "������� ClearCommError ������������� ����� ����������� ������: "+ ErrorsMaskToStr(commErrors)+"\n" );
            return 0;
        }

        return commStat.cbInQue;
    }
    //---------------------------------------------------------------------
    // ��������
    int Port::Write( void* pBuff, int buffSize )
    {
        unsigned long writenCount = 0;
        MY_SAFE_CALL_WIN_API(WriteFile,( hComm_, pBuff, buffSize, &writenCount, NULL));
        if( (int)writenCount!=buffSize )
    	{
        	const AnsiString errMsg = AnsiString().sprintf(
            	"%s, ������ ������ - �������� %d ���� �� %d!\n",
            Description(), writenCount, buffSize );

            MyWCout( errMsg);
    	}
        return writenCount;
    }
    //---------------------------------------------------------------------
    // �������
    int Port::Read(  void* pBuff, int buffSize )
    {
        DWORD readedCount = 0;
        MY_SAFE_CALL_WIN_API(ReadFile,(hComm_, pBuff, buffSize, &readedCount, NULL ));
        return readedCount;
    }

    //---------------------------------------------------------------------
    const Setings& Port::GetSettings() const
    {
    	return sets_;
    }
    //-------------------------------------------------------------------------
    void Port::SetSettings(const Setings& sets)
    {
    	sets_ = sets;
        ApplyBuffsSizes();
        ApplyDCB();
        ApplyTimeOuts();
    }
    //----------------------------------------------------------------------
    void Port::SetSettingsDefault()
    {
        DCB dcb;
        SetDefaultDCB(dcb);
        COMMTIMEOUTS timeOuts;
        SetDefaultCommTimeOuts(timeOuts);
        SetBuffsSizes(defaultIOBuffSize, defaultIOBuffSize);
        SetDCB(dcb);
        SetTimeOuts( timeOuts );
    }
    //-------------------------------------------------------------------------
    AnsiString Port::GetPortName() const
    {
    	return portName_;
    }
    //-------------------------------------------------------------------------
    void Port::SetPortName(const AnsiString &portName)
    {
    	if( portName_== portName ) return;
        portName_= portName;
    	const bool wasConnected = IsConnected();
    	Disconnect();
        if( wasConnected )
        	Connect();
    }
    //-------------------------------------------------------------------------
    void Port::SetBuffsSizes(DWORD txdBuffSize, DWORD rxdBuffSize)
    {
    	if(sets_.txdBuffSize_==txdBuffSize && sets_.rxdBuffSize_==rxdBuffSize)
        	return;
        sets_.txdBuffSize_=txdBuffSize;
        sets_.rxdBuffSize_=rxdBuffSize;
        ApplyBuffsSizes();
    }
    //-------------------------------------------------------------------------
    void Port::SetDCB(const DCB& dcb)
    {
    	sets_.dcb_ = dcb;
        sets_.dcb_.DCBlength = sizeof(sets_.dcb_);
        sets_.dcb_.wReserved = 0ul;
        ApplyDCB();
    }
    //-------------------------------------------------------------------------
    void Port::SetTimeOuts( const COMMTIMEOUTS& timeOuts )
    {
    	sets_.timeOuts_ = timeOuts;
        ApplyTimeOuts();
    }
    //-------------------------------------------------------------------------
    /**  �������� ������ ��� ��������� ������������ �����
         hwnd - ����, ��� �������� ������ ������ ���������
         getFromSystem: ���� false, ���������������� ������
         �������� ����������� �����, ����� �����������,
         ����������� � ������� GetCommConfig
    */
    //-------------------------------------------------------------------------
    void Port::SetupDialog(HWND hwnd, bool getFromSystem)
    {
    	OPEN_PORT_IN_THIS_MEM_FOO;
        COMMCONFIG cmCfg = GetConfig(hComm_);
        if( !getFromSystem )
        {
        	cmCfg.dcb = sets_.dcb_;
            try
            {
            	SetConfig( hComm_, cmCfg );
                MY_SAFE_CALL_WIN_API(SetCommState,(hComm_, &sets_.dcb_ ) );
            }
            catch(...)
            {
            	SetDefaultDCB(cmCfg.dcb);
                SetDefaultDCB(sets_.dcb_);
                SetConfig( hComm_, cmCfg );
                MY_SAFE_CALL_WIN_API(SetCommState,(hComm_, &sets_.dcb_ ) );
            }
        }
        if( CommConfigDialog( portName_.c_str(), hwnd, &cmCfg ) )
        {
        	SetConfig( hComm_, cmCfg );
            sets_.dcb_ = cmCfg.dcb;
            MY_SAFE_CALL_WIN_API(SetCommState,(hComm_, &sets_.dcb_ ) );
        }
    }
    //-------------------------------------------------------------------------
    COMMTIMEOUTS Port::ReciveTimeOuts()
    {
    	OPEN_PORT_IN_THIS_MEM_FOO;
        MY_SAFE_CALL_WIN_API(GetCommTimeouts,(hComm_, &sets_.timeOuts_) );
        mustApplyTimeOuts_ = false;
        return sets_.timeOuts_;
    }
    //-------------------------------------------------------------------------
    DCB Port::ReciveDCB()
    {
    	OPEN_PORT_IN_THIS_MEM_FOO;
        MY_SAFE_CALL_WIN_API(GetCommState,(hComm_, &sets_.dcb_) );
        mustApplyDCB_ = false;
        return sets_.dcb_;
    }
    //-------------------------------------------------------------------------
    AnsiString Port::Description() const
    {
    	AnsiString debugInfo =
        AnsiString().sprintf( "%s %d", portName_, sets_.dcb_.BaudRate );
        return debugInfo;
    }
    //---------------------------------------------------------------------
    void Port::EscapeComm( DWORD dwFunc )
    {
    	MY_SAFE_CALL_WIN_API(EscapeCommFunction,(hComm_, dwFunc) );
    }
    //---------------------------------------------------------------------
    

    // �������� �������� ������ �����-������ ���-�����
	typedef  std::pair<DWORD,std::string> CommErrorT;
	const CommErrorT commErrors[] =
	{
    	CommErrorT(CE_BREAK, "The hardware detected a break condition."),
		CommErrorT(CE_FRAME, "The hardware detected a framing error."),
		CommErrorT(CE_IOE, "An I/O error occurred during communications with the device."),
		CommErrorT(CE_MODE, "The requested mode is not supported, or the hFile parameter is invalid."),
		CommErrorT(CE_OVERRUN,"A character-buffer overrun has occurred. The next character is lost."),
		CommErrorT(CE_RXOVER, "An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character."),
		CommErrorT(CE_RXPARITY, "The hardware detected a parity error."),
		CommErrorT(CE_TXFULL, "The application tried to transmit a character, but the output buffer was full.")
	};
    AnsiString ErrorsMaskToStr( DWORD commErrorsMask )
    {

    	AnsiString ret;
    	for( unsigned i=0; i<array_size(commErrors);++i )
        	if( commErrorsMask & commErrors[i].first ) {
                AnsiString s;
                s.sprintf("%d, %s; ", commErrors[i].first, commErrors[i].second.c_str());
            	ret = ret + s;
            }
        return ret;
    }
    //---------------------------------------------------------------------
    // �������� �� ������ �����������
    bool HasMaskCriticalError( DWORD commErrorsMask )
    {
    	for( unsigned i=0; i<array_size(commErrors);++i )
        	if( commErrorsMask & commErrors[i].first )
            	return true;
        return false;
    }
}; // namespace Comm
}; //namespace my


