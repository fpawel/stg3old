//---------------------------------------------------------------------------
#ifndef RS232ModbusH
#define RS232ModbusH
//---------------------------------------------------------------------------
//std

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// my
#include "..\my_include\free_types_.h"
#include "MyExeption.h"
#include "winRS232.h"

// ���������� ����������
#include "MySynchronize.h"
#include "MyIostream.h"


	/**
	*		���������� MyTransferException - ������� ��� ���� ���������� ������
	*/
    class MyTransferException : public MyExternalException
    {
    public:
        virtual ~MyTransferException() {}
        virtual AnsiString ClassName() const { return "MyTransferException"; }
        unsigned Slave() const { return slave_; }

    protected:
        MyTransferException(unsigned slave, const FileLine& loc, const AnsiString& msg, const PMyExcpt nest ) :
            MyExternalException(loc, "����� $"+IntToHex( (int)slave,2)+" "+msg, nest ), slave_(slave)
        {   
        }
    private:
        const unsigned slave_;
    };


    bool IsMyTransferException(const MyException* e)
    {
        return e->TypeOf<MyTransferException>();
    }

    #define DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, THENAME )\
    class My##THECLASS##Exception : public My##THEBASECLASS\
    {\
    	friend struct MyExcptionFactory;\
    protected:\
    	My##THECLASS##Exception(unsigned slave, const FileLine& loc, const AnsiString& msg,\
        	PMyExcpt nest ) :\
 		My##THEBASECLASS##(slave, loc, msg, nest){}\
    public:\
    	virtual ~My##THECLASS##Exception(){}\
		virtual AnsiString ClassName() const { return THENAME; }\
    }

    #define DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_(THECLASS, THEBASECLASS )\
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, "My"#THECLASS"Exception" )



    /**
	*		���������� MyTransferCantOpenPortException
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferCantOpenPort, ExternalException );

    /**
	*		���������� MyBadAnswerException
	*/
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_( TransferBadAnswer, TransferException );

    /**
	*		���������� MyTransferNoAnswerException
	*/
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_( TransferNoAnswer, TransferBadAnswerException );

    /**
	*		���������� MyTransferAnswerLengthMismatchException
	*/
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_( TransferAnswerLengthMismatch, TransferBadAnswerException );

    /**
	*		���������� MyTransferAnswerAdressMismatchException
	*/
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_( TransferAnswerAdressMismatch, TransferBadAnswerException );

    /**
	*		���������� MyTransferAnswerCommandCodeMismatchException
	*/
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_( TransferAnswerCommandCodeMismatch, TransferBadAnswerException );

    /**
	*		���������� MyTransferBadCRC16Exception
	*/
    DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_( TransferBadCRC16, TransferBadAnswerException );

    #undef DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, THENAME )
    #undef DECLARATE_AND_DEFINE_TRANSFER_EXCEPTION_CLASS_(THECLASS, THEBASECLASS )


class RS232ModbusT : public boost::noncopyable
{
public:
    
    explicit RS232ModbusT();
    ~RS232ModbusT();

    // �������� txd, ������� ������ �������� � rxd,
    // ������� GetCRC16(rxd)==0
    unsigned PerformTransfer
    (
    	unsigned char modbusDeviceAddress,
    	unsigned char commandCode,			// ��� ��������
        ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    	ByteBuffPtr sendEnd,
    	ByteBuffPtr& reciveBegin,			// ������ �� �������� ������
        ByteBuffPtr& reciveEnd
    );

    const AnsiString GetPort() const { return comm_.GetPortName(); }
    void SetPort( AnsiString s )
    {
        comm_.SetPortName(s);
    }
    void SetupDialog();

    void SetCheckCancelTransferCallBack( TBoolMthd checkCancelTransfer )
    {
    	checkCancelTransfer_ = checkCancelTransfer;
    }

    void SetReportStateCallBack( TVoid_IntStrMthd reportState )
    {
    	reportState_ = reportState;
    }

private:
    const boost::shared_ptr<void> logLifeTime_;
	TBoolMthd checkCancelTransfer_;
    TVoid_IntStrMthd reportState_;

    VInt8 rxd_;
    my::RS232::Port comm_;

    unsigned tmWriteDelay_, timeOut_, silentTime_, rxdMaxFrameSize_;
    bool mustLogData_;

    void TryPerformTransfer(const VInt8& txd, VInt8& rxd);

    // ������� ����� ����� � �������� �����������
    void OpenChanal();
    // ������� ������� ����������
    void CloseChanal();

    bool NoBreakTransfer(){ return false; }
    void NoReport( unsigned, const AnsiString&  ) {}
    void LogOut( const AnsiString& );
    void SyncronizedSleep( unsigned tm );
};





//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

