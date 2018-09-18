//---------------------------------------------------------------------------

#include <iterator>
//------------------------------------------------------------------------------
#pragma hdrstop

#include "task_.h"

#include "PerformTransfer.h"
#include "bcd_.h"
#include "Modbus_.h"
#include "MySynchronize.h"
#include "MyExeption.h"
#include "my_array_size.h"
#include "AnsiStringUtils_.h"
#include "TransferManage.h"
#include "Unit1.h"
#include "modbus_.h"
#include "RS232Modbus.h"
#include "uFormSearchSlaves.h"

//vcl
#include <DateUtils.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
using std::cout;
//---------------------------------------------------------------------------
void HandleSTG3Task::Perform() VIRTUAL_FINAL_
{
	try
    {
    	PerformSTG3();
    } catch(...)
    {
    	PMyExcpt e = MY_RETHROW_;
        if( const MyTransferNoAnswerException *ee =
        	dynamic_cast<MyTransferNoAnswerException*>( e.get() ) )
        {
            if( my::Synchronize1( &DoIfNoAnswer, ee->Slave() )==0)
        		throw e;
        }
        else
        	throw e;
    }
}
//---------------------------------------------------------------------------
LRESULT HandleSTG3Task::DoIfNoAnswer(unsigned slave)
{
	return HandleNotAnswer(slave);
}
//---------------------------------------------------------------------------

HandleSearchSTG3Task::HandleSearchSTG3Task() : MyAbstructTransfer(), slaves_( FormSearchSlaves->GetLeftSlaves() )
{
}


HandleSearchSTG3Task::~HandleSearchSTG3Task()
{
	//Form1->pnSearchDevice->Visible = 0;
}
//---------------------------------------------------------------------------
AnsiString HandleSearchSTG3Task::What() const
{
    return "Поиск по адрессам.";
}
//---------------------------------------------------------------------------
void HandleSearchSTG3Task::Perform() VIRTUAL_FINAL_
{
	while( !slaves_.empty() )
    {
		const uInt8 slave = slaves_.back();
    	ByteBuffPtr rxd = 0, rxdEnd = 0;
        bool answered = false;

        try
    	{
    		ReadModbusRegistersData(  slave, 0,	1, rxd, rxdEnd );
            answered = true;
    	} catch(...)
    	{
    		PMyExcpt e = MY_RETHROW_;
        	if( !e->TypeOf<MyTransferNoAnswerException>() )
        		throw e;
        }
    	my::Synchronize2( &FormSearchSlaves->OnSearchAddr, slave, answered );
        slaves_.pop_back();
    }

}
//---------------------------------------------------------------------------
bool HandleSearchSTG3Task::HandleBeginTransfer()				VIRTUAL_FINAL_
{
	return true;
}
//---------------------------------------------------------------------------
bool HandleSearchSTG3Task::HandleEndTransfer()				VIRTUAL_FINAL_
{
	FormSearchSlaves->ModalResult = mrOk;
	return false;
}
//------------------------------------------------------------------------------
HandleSTG3MeasurementsTask::HandleSTG3MeasurementsTask(unsigned slave) :
	HandleSTG3Task(), slave_(slave), hasGot_(false)
{
}
//------------------------------------------------------------------------------
AnsiString HandleSTG3MeasurementsTask::What() const
{ return "$"+IntToHex((int)slave_,2)+": запрос концентрации"; }
//------------------------------------------------------------------------------
void HandleSTG3MeasurementsTask::PerformSTG3() VIRTUAL_FINAL_
{
    ByteBuffPtr rxd = 0, rxdEnd = 0;

    ReadModbusRegistersData(  slave_, 0, 2, rxd, rxdEnd );
    BCDValue x = BCDToFloat(rxd);
    conc_ = x.Value;
    porog1_ = x.Porog1;
    porog2_ = x.Porog2;
    hasGot_ = true;
}
//------------------------------------------------------------------------------
bool HandleSTG3MeasurementsTask::HandleBeginTransfer() VIRTUAL_FINAL_
{   
    hasGot_ = false;
	return true;
}
//------------------------------------------------------------------------------
bool HandleSTG3MeasurementsTask::HandleEndTransfer()				VIRTUAL_FINAL_
{
	if( !hasGot_ ) {
    	Form1->SetNotAnswered(slave_);
        Form1->SetPorog1(slave_, false);
        Form1->SetPorog2(slave_, false);
    }
    else {
    	Form1->SetSTG3Conc(slave_, conc_);
        Form1->SetPorog1(slave_, porog1_);
        Form1->SetPorog2(slave_, porog2_);
    }
    return true;
}
//------------------------------------------------------------------------------
bool HandleSTG3MeasurementsTask::HandleNotAnswer(unsigned slave)	VIRTUAL_FINAL_
{
	hasGot_ = false;
    Form1->SetNotAnswered(slave);
    return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
HandleSTG3WriteTask::HandleSTG3WriteTask(unsigned slave, unsigned paramIndex, double val) :
	MyAbstructTransfer(), slave_(slave), paramIndex_(paramIndex),
    writeCmdCode_(STG3::WRITE_CMD_CODE[paramIndex_]),
    val_(val), writen_(false)
{
	assert( paramIndex_<STG3::PRMS_COUNT );
}
//------------------------------------------------------------------------------
AnsiString HandleSTG3WriteTask::What() const
{
	return AnsiString().sprintf("$%s:$%s: установка \"%s\"=%s",
    ::IntToHex( (int)slave_,2),
    ::IntToHex( (int)writeCmdCode_, 2),
    STG3::PRM_NAME[paramIndex_], FloatToStr(val_) );
}
//------------------------------------------------------------------------------
void HandleSTG3WriteTask::Perform() VIRTUAL_FINAL_
{
	try
    {
    	WriteModbusFloat( slave_, writeCmdCode_, val_ );
        writen_ = true;
    } catch(...)
    {
    	PMyExcpt e = MY_RETHROW_;
        if( !e->TypeOf<MyTransferNoAnswerException>() )
        	throw e;
        my::Synchronize1( &ShowErrorMessage,
        "Ошибка выполнения команды \""+What()+"\"!\nНет ответа!" );
    }




}
//------------------------------------------------------------------------------
bool HandleSTG3WriteTask::HandleBeginTransfer() VIRTUAL_FINAL_
{
	return true;
}
//------------------------------------------------------------------------------
bool HandleSTG3WriteTask::HandleEndTransfer()				VIRTUAL_FINAL_
{
	return false;
}
//------------------------------------------------------------------------------
LRESULT HandleSTG3WriteTask::ShowErrorMessage(const AnsiString& msg)
{
	if( Form1!=NULL )
	::MessageBox(Form1->Handle,	msg.c_str(), THIS_APPLICATION_TITLE,
    	MB_ICONERROR | MB_SYSTEMMODAL | MB_OK );
	return 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
HandleSTG3SetSlaveTask::HandleSTG3SetSlaveTask(unsigned slave) :
	MyAbstructTransfer(), slave_(slave)
{
}
//------------------------------------------------------------------------------
AnsiString HandleSTG3SetSlaveTask::What() const
{
	return "Установить сетевой адресс $"+IntToHex( (int)slave_, 2);
}
//------------------------------------------------------------------------------
void HandleSTG3SetSlaveTask::Perform() 					VIRTUAL_FINAL_
{
    ByteBuffPtr rxd = 0, rxdEnd = 0;

	// установить адресс
    /** */
    WriteModbusFloat(  0, 0x8047, slave_ );
    Sleep(1000);
    ReadModbusRegistersData(  slave_, 0, 2, rxd, rxdEnd );	
}
//------------------------------------------------------------------------------
bool HandleSTG3SetSlaveTask::HandleBeginTransfer() 		VIRTUAL_FINAL_
{
	return true;
}
//------------------------------------------------------------------------------
bool HandleSTG3SetSlaveTask::HandleEndTransfer()			VIRTUAL_FINAL_
{
	Form1->SetAnswered(slave_);
	return false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
HandleSTG3ReadModbustTableTask::HandleSTG3ReadModbustTableTask(unsigned slave) :
	HandleSTG3Task(), slave_(slave), hasGot_(false)
{
	std::fill( dt_, dt_ + STG3::PRMS_COUNT, 0 );
}
//------------------------------------------------------------------------------
AnsiString HandleSTG3ReadModbustTableTask::What() const
{ return "$"+IntToHex((int)slave_,2)+": запрос установленных значений"; }
//------------------------------------------------------------------------------
void HandleSTG3ReadModbustTableTask::PerformSTG3() VIRTUAL_FINAL_
{
    ByteBuffPtr rxd = 0, rxdEnd = 0;
    using namespace STG3;
    for( unsigned prmIdx = 0; prmIdx<PRMS_COUNT; ++prmIdx )
    {
    	ReadModbusRegistersData( slave_, MODBUS_REG[prmIdx], 2, rxd, rxdEnd );
    	dt_[prmIdx] = BCDToFloat(rxd).Value;
    }
    hasGot_ = true;
}
//------------------------------------------------------------------------------
bool HandleSTG3ReadModbustTableTask::HandleBeginTransfer() VIRTUAL_FINAL_
{
    hasGot_ = false;
	return true;
}
//------------------------------------------------------------------------------
bool HandleSTG3ReadModbustTableTask::HandleEndTransfer()				VIRTUAL_FINAL_
{
	if( !hasGot_ )
    {
    	Form1->SetNotAnswered(slave_);
        return true;
    }
    Form1->SetAnswered(slave_);
    Form1->SetSTG3Values(slave_, dt_);
    return false;
}
//------------------------------------------------------------------------------
bool HandleSTG3ReadModbustTableTask::HandleNotAnswer(unsigned slave)	VIRTUAL_FINAL_
{
	hasGot_ = false;
    Form1->SetNotAnswered(slave);
    return true;
}
//------------------------------------------------------------------------------
