//---------------------------------------------------------------------------
#ifndef TransferManageH
#define TransferManageH

// ���������� ����������
#include "MySynchronize.h"


// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"


// my
#include "MyAbstructTransfer.hpp"
#include "..\my_include\free_types_.h"
#include "..\VCLUtils\VCLSignal.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ��� ����� ������� ��������
class TFormCtrlPrgrmmDlg;
typedef boost::shared_ptr<TFormCtrlPrgrmmDlg> PFormCtrlPrgrmmDlg;
//---------------------------------------------------------------------------
typedef my::VCLSignal::Signal<TVoid_IntStrMthd> TReportStateSignal;
//---------------------------------------------------------------------------
// ������ ����������
class TaskList;


class TransferManagerT : public boost::noncopyable
{
public:
    enum{ ADD_TASK_HEAD = false, ADD_TASK_TAIL = true };

    // ��������� ������� - �������� �������
	void AddTask( MyAbstructTransfer *trnsfr, bool addTail = ADD_TASK_TAIL );

	void AddSingleBackgroundTask( void (__closure *aMethod )(),
    	const AnsiString& what,  bool addTail = ADD_TASK_TAIL );

	// ���������� �������
    void StartSimple();
	void StartSearchSlaves();
	void StopThread();
    bool IsThreadStarted() const;

	boost::shared_ptr<void> ConnectOnReportState( TVoid_IntStrMthd method );

    void DebugEditList();

    /**
	*		��������� �������� �������������.
	*/
	unsigned PerformTransfer
	(
    	uInt8 modbusAddr,
		uInt8 commandCode,			// ��� ��������
		ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    	ByteBuffPtr sendEnd,
    	ByteBuffPtr& reciveBegin,
    	ByteBuffPtr& reciveEnd
	);

    /**
    *       ������ ����������
    */
    TaskList& GetTaskList();

    void SetupDialog();
    void SyncronizedSleep( unsigned tm );

private:
	class Impl;
    boost::shared_ptr<Impl> impl_;
    explicit TransferManagerT();
    friend TransferManagerT& TransferManager();
};
//------------------------------------------------------------------------------
TransferManagerT& TransferManager();
//------------------------------------------------------------------------------


#endif
