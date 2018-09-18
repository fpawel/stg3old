#include <map>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "TransferManage.h"


#include <windows.h>
#include <vcl.h>

// loki
//#include "Loki\threads.h"


#include "..\VCLUtils\myThread.h"
#include "TaskList.h"
#include "MyExeption.h"
#include "uFrmWait.h"
#include "AnsiStringUtils_.h"
#include "RS232Modbus.h"
#include "..\appcfg.h"
#include "Unit1.h"
#include "task_.h"

// ��� ����������� ������� PerformModbusTransfer
#include "PerformTransfer.h"


#pragma package(smart_init)
#pragma option -xp
//------------------------------------------------------------------------------
using std::cout;
using std::endl;
//------------------------------------------------------------------------------
typedef std::map<AnsiString,AnsiString> TranslateMap;
TranslateMap MakeExceptionTranslateMap()
{
    TranslateMap ret;
    ret["MyTransferCantOpenPortException"] = "���� ������������� ������ �����";
    ret["MyNoAnswerException"] = "������ �� ��������";
    ret["MyTransferAnswerLengthMismatchException"] = "�������� ����� �������";
    ret["MyTransferAnswerAdressMismatchException"] = "������������ ��������";
    ret["MyTransferAnswerCommandCodeMismatchException"] = "������������ ����� ��������";
    ret["MyTransferBadCRC16Exception"] = "��������� ����������� �����";
    ret["MyTransferNoAnswerException"] = "������ �� ��������";
    return ret;
}
//------------------------------------------------------------------------------
const AnsiString& TranslateExceptionType(const AnsiString& s)
{
    const static TranslateMap translateMap = MakeExceptionTranslateMap();
    const static AnsiString strNo = ""; 
    typedef TranslateMap::const_iterator Ptr;
    const Ptr fnd = translateMap.find( s );
    return fnd==translateMap.end() ? strNo : fnd->second;
}
//------------------------------------------------------------------------------
AnsiString FormatMyException( const MyException* excpt )
{
    return AnsiString().sprintf("%s %d %s", excpt->Location().second,
        excpt->Location().first, excpt->Message() );
}
//---------------------------------------------------------------------------
AnsiString FormatMyExceptionStack( const MyException* excpt )
{
	AnsiString ret = "";
	for ( ; excpt; excpt=excpt->Nested() )
	    ret = ret + FormatMyException(excpt) + " ";
	return ret;
}
//---------------------------------------------------------------------------


class TransferManagerT::Impl : public boost::noncopyable
{
public:
    explicit Impl();
    ~Impl();
    //--------------------------------------------------------------------------
    // ���������� �������
    void StartSimple();
	void StartSearchSlaves();
    void StopThread();
    bool IsThreadStarted() const;
    //--------------------------------------------------------------------------
	TReportStateSignal::Connections& ReportStateConnections()
    { return signalReportState_.GetConnections(); }
    //---------------------------------------------------------------------------
    TaskList& GetTaskList() { return taskList_; }
    //---------------------------------------------------------------------------
    void DebugEditList();
    //---------------------------------------------------------------------------
    void DebugConsoleInput();

    // ��������� �������� �������������.
	unsigned PerformTransfer( uInt8 modusAddr, uInt8 commandCode, ByteBuffPtr sendBegin,
    	ByteBuffPtr sendEnd, ByteBuffPtr& reciveBegin, ByteBuffPtr& reciveEnd )
    {
    	return modbus_->PerformTransfer(modusAddr, commandCode, sendBegin, sendEnd, reciveBegin, reciveEnd);
    }

    // ��������� ������� - �������� �������
    void AddTask( MyAbstructTransfer *trnsfr, bool addTail = ADD_TASK_TAIL)
    {
        if( addTail==ADD_TASK_TAIL )
            taskList_.PushBackTail( PTransfer(trnsfr) );
        else
            taskList_.PushFrontHead( PTransfer(trnsfr) );
    }

    void SetupDialog()
    {
    	const bool isThreadSatrted = !thr_->IsTerminated();
    	if(isThreadSatrted) StopThread();
    	modbus_->SetupDialog();
    	if(isThreadSatrted) StartSimple();
    }

    void SyncronizedSleep( unsigned tm )
    {
    	const unsigned tmStart = ::GetTickCount();
        while( GetTickCount()-tmStart < tm && !thr_->IsTerminated() ) ;
    }

private:
    boost::shared_ptr<void> logLifeTime_;
    //--------------------------------------------------------------------------
    //  �����
    boost::shared_ptr<my::Thread> thr_;
	//--------------------------------------------------------------------------
    // ������ ���������� ������
    boost::shared_ptr<RS232ModbusT> modbus_;
    //--------------------------------------------------------------------------
    // ������ ����������� ����������
    TaskList taskList_;
    //--------------------------------------------------------------------------
    TReportStateSignal signalReportState_;
    //--------------------------------------------------------------------------
    // ��������� ��������������� ����������
    PMyExcpt lastExcpt_;
    //--------------------------------------------------------------------------
    void StartThread();
	// ��������� ����������
	void Execute();
    //--------------------------------------------------------------------------
    // ��������� �����
    LRESULT SignalReportState(unsigned context, const AnsiString& s)
    { signalReportState_.Signal2(context, s); return 0; }
    //--------------------------------------------------------------------------
    // ��������� �����
    void ReportStateFromThread( unsigned context, const AnsiString& s)
    {
    	my::Synchronize2( &SignalReportState, context, s );
    }
    //--------------------------------------------------------------------------
    // ������ �������� ���������� - ����� �������� �� ������
    void CatchCurentException();
    //--------------------------------------------------------------------------
    //����������� ������� �� ������ - �������� �� ������
    PTransfer PrepareCurentTask();
    //--------------------------------------------------------------------------
    //              ������� �� �������
    LRESULT HandleBeginTransfer();
    LRESULT HandleEndTransfer();
    LRESULT HandleCurentException();

};
//------------------------------------------------------------------------------
TransferManagerT::Impl::Impl() :
    logLifeTime_( MakeMyLogQuote("MyTransfer::Manager.[members]") ),
    // ������� ������ ������� ������
	thr_( new my::Thread(&Execute) ),
    signalReportState_(),
    modbus_( new RS232ModbusT )
{
    LOG_FOO_LIFETIME_QUOTES;
    thr_->SetPriority( static_cast<TThreadPriority> (
        IniConfig()->ReadInteger(TRANSFERS_SEKT, TRANSFERS_THREAD_PRIORITY_ID, tpLowest)
        ) );
    modbus_->SetCheckCancelTransferCallBack( &thr_->IsTerminated );
    modbus_->SetReportStateCallBack( &ReportStateFromThread );
    //MY_FULL_TRACE( "Starting transfers on  " << intrfc_->Chanl()->GetDebugInfo() << endl);
}
//------------------------------------------------------------------------------
TransferManagerT::Impl::~Impl()
{
	LOG_FOO_LIFETIME_QUOTES;
    try
    {

        IniConfig()->WriteInteger(TRANSFERS_SEKT, TRANSFERS_THREAD_PRIORITY_ID, thr_->GetPriority() );
        signalReportState_.GetConnections().DisconnectAll();
        assert( thr_->WasStoped() );
    }
    catch (...)
    {
        assert(false);
    }

}
//------------------------------------------------------------------------------
// ���������� �������
void TransferManagerT::Impl::StartThread()
{
	LOG_FOO_LIFETIME_QUOTES;
    my::FixMainThread();
    lastExcpt_.reset();
    thr_->Run();
}
void TransferManagerT::Impl::StartSimple()
{
	if( IsThreadStarted() ) StopThread();
    const VInt8 slaves = Form1->GetSlaves();
    assert( !slaves.empty() );
    for( unsigned i=0; i<slaves.size(); ++i )
    {
        AddTask( new HandleSTG3ReadModbustTableTask( slaves[i] ) );
        AddTask( new HandleSTG3MeasurementsTask( slaves[i] ) );    
    }
    StartThread();
}

void TransferManagerT::Impl::StartSearchSlaves()
{
	if( IsThreadStarted() ) StopThread();
    taskList_.Clear();
    AddTask( new HandleSearchSTG3Task );
    StartThread();
}


//------------------------------------------------------------------------------
void TransferManagerT::Impl::StopThread()
{
    LOG_FOO_LIFETIME_QUOTES;

    boost::shared_ptr<TFrmWait> formWait( new TFrmWait(NULL) );
    formWait->Show();
    formWait->Repaint();

    const TThreadPriority threadPriority = thr_->GetPriority();
    thr_->SetPriority( tpTimeCritical );
    thr_->Stop();
    thr_->SetPriority( threadPriority );
    lastExcpt_.reset();

    taskList_.Clear();
    ::Sleep( 200  );
}

bool TransferManagerT::Impl::IsThreadStarted() const{	return !thr_->WasStoped(); }

//------------------------------------------------------------------------------
void TransferManagerT::Impl::CatchCurentException()
{
    const PMyExcpt excpt = RethrowMyException(GET_FLINE_);
    if( lastExcpt_.get()!=NULL && lastExcpt_->Root()->Location()==excpt->Root()->Location() ) return;
    lastExcpt_ = excpt;
    my::SynchronizeMthd( &HandleCurentException );
}
//------------------------------------------------------------------------------
void TransferManagerT::Impl::Execute()
{
    LOG_FOO_LIFETIME_QUOTES;
    ReportStateFromThread( RPT_TRNSFR_CNTXT::START_THREAD, "�����: �����" );
	for
    (
    	PTransfer curentTask = PrepareCurentTask();
        !thr_->IsTerminated();
        curentTask = PrepareCurentTask()
    )
    {

    	try
    	{
    	    if(curentTask.get()==NULL)
            {
            	if( taskList_.IsEmpty() ) break; else continue;
            }

            //��������� ������� ������ ���������
    		if( my::SynchronizeMthd( &HandleBeginTransfer )==false )
            {
            	taskList_.PopFrontBody();
                continue;
            }

            // ��������� ��������
            //intrfc_->OpenChanal();
            curentTask->Perform();
            if( thr_->IsTerminated() ) break;

            //��������� ������� ��������� ���������
            if( my::SynchronizeMthd(  &HandleEndTransfer )==true )
                taskList_.PushBackBody( curentTask ) ;
            taskList_.PopFrontBody();

        }// try
   	 	catch(...)
    	{
        	CatchCurentException();
            SyncronizedSleep(1000);
    	}
    } // for( PTaskData curentTask = taskList_.Front();
    
    my::SynchronizeWCout( "����� ����������.\n" );
    ReportStateFromThread( RPT_TRNSFR_CNTXT::STOP_THREAD, "����� ����������" );
}
//---------------------------------------------------------------------------
// ������� �� ������� "������ ���������"
LRESULT TransferManagerT::Impl::HandleBeginTransfer()
{
	if( thr_->IsTerminated() ) return 0;
	const PTransfer curentTask = taskList_.FrontBody();
    assert(curentTask);
    const LRESULT ret = static_cast<LRESULT>( curentTask->HandleBeginTransfer() );
    if(ret!=0)
    	SignalReportState(RPT_TRNSFR_CNTXT::BEG, curentTask->What() );
	return ret;
}
//---------------------------------------------------------------------------
// ������� �� ������� "��������� ���������"
LRESULT TransferManagerT::Impl::HandleEndTransfer()
{
	if( thr_->IsTerminated() ) return 0;
	const PTransfer curentTask = taskList_.FrontBody();
    assert(curentTask);
    SignalReportState( RPT_TRNSFR_CNTXT::END, curentTask->What() );
    lastExcpt_.reset();
    return static_cast<LRESULT>( curentTask->HandleEndTransfer() );
}
//---------------------------------------------------------------------------
LRESULT TransferManagerT::Impl::HandleCurentException()
{
	if( thr_->IsTerminated() || !lastExcpt_.get() ) return 0;

    AnsiString msg = "������: ";
    msg = msg + TranslateExceptionType( lastExcpt_->ClassName() );

    my::SynchronizeWCout( "������ - "+lastExcpt_->Root()->Message() + "\n");
    SignalReportState(RPT_TRNSFR_CNTXT::ERR, lastExcpt_->Message() );
    return 0;
}
//---------------------------------------------------------------------------
//����������� ������� �� ������ - �������� �� ������
PTransfer TransferManagerT::Impl::PrepareCurentTask()
{
	assert(thr_);
    if( thr_->IsTerminated() ) return PTransfer();
    my::SynchronizeMthd( &taskList_.Merge );
    PTransfer ret  = taskList_.FrontBody();
    return ret;
}
//------------------------------------------------------------------------------
void TransferManagerT::Impl::DebugEditList()
{
    LOG_FOO_LIFETIME_QUOTES;
    const bool isThreadSatrted = !thr_->IsTerminated();
    if(isThreadSatrted) StopThread();
    AddTask( CreateSingleBackgroundTask(&taskList_.DebugEdit, "Debug edit list" ), ADD_TASK_HEAD);
    if(isThreadSatrted) StartThread();
}
//------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////
//					class TransferManagerT
/////////////////////////////////////////////////////////////////////////////////
TransferManagerT::TransferManagerT() : impl_(new Impl)
{
}
//------------------------------------------------------------------------------
boost::shared_ptr<void> TransferManagerT::ConnectOnReportState(TVoid_IntStrMthd method)
{
	return impl_->ReportStateConnections().AutoConnect(method);
}
//------------------------------------------------------------------------------
// ��������� ������� - �������� �������
void TransferManagerT::AddTask( MyAbstructTransfer *trnsfr, bool addTail)
//bool addTail = ADD_TASK_TAIL
{
    impl_->AddTask(trnsfr, addTail);
}
//---------------------------------------------------------------------------
void TransferManagerT::AddSingleBackgroundTask( void (__closure *aMethod )(),
	const AnsiString& what, bool addTail )
//bool addTail = ADD_TASK_TAIL
{
    impl_->AddTask( CreateSingleBackgroundTask(aMethod, what ) );
}
//---------------------------------------------------------------------------
// ���������� �������
void TransferManagerT::StartSimple() { impl_->StartSimple(); }
void TransferManagerT::StartSearchSlaves() { impl_->StartSearchSlaves(); }
//---------------------------------------------------------------------------
void TransferManagerT::StopThread() {  impl_->StopThread();  }
//---------------------------------------------------------------------------
bool TransferManagerT::IsThreadStarted() const{	return impl_->IsThreadStarted(); }
//---------------------------------------------------------------------------
void TransferManagerT::DebugEditList()
{
	impl_->DebugEditList();
}
//---------------------------------------------------------------------------
// ��������� �������� �������������.
unsigned TransferManagerT::PerformTransfer
(
	uInt8 modbusAddr,
	uInt8 commandCode,			// ��� ��������
	ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
)
{
	return impl_->PerformTransfer(modbusAddr, commandCode, sendBegin, sendEnd, reciveBegin, reciveEnd);
}
//---------------------------------------------------------------------------
void TransferManagerT::SetupDialog()
{
	impl_->SetupDialog();
}
//---------------------------------------------------------------------------
/**
*       ������ ����������
*/
TaskList& TransferManagerT::GetTaskList()
{
    return impl_->GetTaskList();
}
//------------------------------------------------------------------------------
void TransferManagerT::SyncronizedSleep( unsigned tm )
{
	impl_->SyncronizedSleep( tm );
}

TransferManagerT& TransferManager()
{
	static TransferManagerT* pRet = NULL;
    if( pRet==NULL )
    	pRet = new TransferManagerT;
    return *pRet;
}
//------------------------------------------------------------------------------
unsigned PerformModbusTransfer
(
	uInt8 slave,
	uInt8 commandCode,			// ��� ��������
	ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
)
{
	return TransferManager().PerformTransfer( slave, commandCode, sendBegin,
    	sendEnd, reciveBegin, reciveEnd	);
}



////////////////////////////////////////////////////////////////////////////////









