//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "CommSets.h"
#include "MyDCBHelper.h"
#include "Loki\singleton.h"
#include "boost/regex.hpp"
#include "Registry.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)




////////////////////////////////////////////////////////////////////////////////
namespace my {	namespace RS232 {

void enumComports(TStrings* ports) {

	TRegistry *reg = new TRegistry(KEY_READ);

	try {
		reg->RootKey = HKEY_LOCAL_MACHINE;
		if (!reg->OpenKey("hardware\\devicemap\\serialcomm", false)) {
			ports->Clear();
		}
		else {
			ports->BeginUpdate();
			try {
				reg->GetValueNames(ports);
				for (int i = ports->Count - 1; i > -1; i--) {
					ports->Strings[i] = reg->ReadString(ports->Strings[i]);
				}
			} catch (...){

			}
			ports->EndUpdate();
		}
	}
	catch (...) {
		reg->CloseKey();
		delete reg;
	}

}

// ����������� ������ ������
struct ModuleVariablesT : public boost::noncopyable
{
	// DCB �� ���������
	DCB defDcb_;
	// �������� �� ���������
	COMMTIMEOUTS defTimeOuts_;
	// ������ ������, �������������� � �������
    explicit ModuleVariablesT();
};
//------------------------------------------------------------------------------
ModuleVariablesT::ModuleVariablesT()
{
	SetDefaultDCB(defDcb_);
    SetDefaultCommTimeOuts(defTimeOuts_);

    COMMCONFIG cmCfg = {0};
    DWORD sz = sizeof(COMMCONFIG);
    // ��������� ����� ���-������ � �������
    for( unsigned i=1; i<20; ++i )
    {
    	const AnsiString portName = "COM"+IntToStr(i);
        if( GetDefaultCommConfig( portName.c_str(),  &cmCfg,  &sz ) )
        {   
            if( defDcb_!=cmCfg.dcb )
            {
            	cmCfg.dcb = defDcb_;
            	SetDefaultCommConfig( portName.c_str(),  &cmCfg,  sz );
            }
        } ;
    }
}
//------------------------------------------------------------------------------
typedef Loki::SingletonHolder
    	<
    		ModuleVariablesT,
        	Loki::CreateUsingNew,
    		Loki::NoDestroy
    	> ModuleVars;
//------------------------------------------------------------------------------




void InitDCB(DCB& dcb)
{
	ZeroMemory( &dcb,sizeof(dcb) );
    dcb.DCBlength = sizeof(dcb);
    dcb.wReserved = 0ul;
}
//-------------------------------------------------------------------------
void SetDefaultDCB(DCB& dcb)
{
	InitDCB(dcb);
    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = CBR_9600;
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;

    dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;

    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fAbortOnError = FALSE;

    dcb.wReserved = 0ul;

    dcb.XonLim = 1024 / 4;
    dcb.XoffLim = dcb.XonLim;

    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.XonChar = 17;
    dcb.XoffChar = 19;
    dcb.ErrorChar = 0xC0;
    dcb.EofChar = 0;
    dcb.EvtChar = 0;
}
//----------------------------------------------------------------------
Setings::Setings() :
	txdBuffSize_(defaultIOBuffSize), rxdBuffSize_(defaultIOBuffSize)
{
	SetDefaultDCB(dcb_);
    SetDefaultCommTimeOuts(timeOuts_);
}

//------------------------------------------------------------------------------
// ���������� �� ���� � idx � �������
bool IsPortPresentedInSystem(const AnsiString& portName)
{
    TStringList *ports = new TStringList;
    enumComports(ports);
    bool r = ports->IndexOf(portName) != -1;
    delete ports;
    return r;
}
//------------------------------------------------------------------------------
const DCB& GetDefaultDCB() { return ModuleVars::Instance().defDcb_;}
//------------------------------------------------------------------------------
const COMMTIMEOUTS& GetDefaultCommTimeOuts() { return ModuleVars::Instance().defTimeOuts_; }
//------------------------------------------------------------------------------

std::ostream& operator<<( std::ostream& strm, const my::RS232::Setings& sets )
{
	using std::endl;
    using namespace my::RS232;
    for( unsigned idx=0; idx<DCB_FIELDS_COUNT; ++idx )
    	strm << GetDCBFieldName(idx) << "=" << GetDCBField(sets.dcb_, idx) << std::endl <<
		#define OUT_TIMEOUT_VAL(name) ""#name << "=" << sets.timeOuts_.##name << endl
        OUT_TIMEOUT_VAL(ReadIntervalTimeout)			<<
        OUT_TIMEOUT_VAL(ReadTotalTimeoutMultiplier)		<<
        OUT_TIMEOUT_VAL(ReadTotalTimeoutConstant)		<<
        OUT_TIMEOUT_VAL(WriteTotalTimeoutMultiplier)	<<
        OUT_TIMEOUT_VAL(WriteTotalTimeoutConstant)		<<
		#undef OUT_TIMEOUT_VAL(name)
		"txdBuffSize=" << sets.txdBuffSize_  		<<
        " rxdBuffSize="  << sets.rxdBuffSize_		<<
    	endl
	;
	return strm;
}
}; }; // namespace my {	namespace RS232 {
////////////////////////////////////////////////////////////////////////////////

