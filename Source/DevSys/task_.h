//---------------------------------------------------------------------------
#ifndef task_H
#define task_H
//---------------------------------------------------------------------------

//my
#include "MyAbstructTransfer.hpp"
#include "appcfg.h"

#define VIRTUAL_FINAL_ //
//---------------------------------------------------------------------------
class TransferManager;
//---------------------------------------------------------------------------
class HandleSTG3Task : public MyAbstructTransfer
{
public:
    virtual ~HandleSTG3Task(){}
private:
    void Perform(); 				VIRTUAL_FINAL_
    LRESULT DoIfNoAnswer(unsigned slave);
    virtual void PerformSTG3() = 0;
    virtual bool HandleNotAnswer(unsigned slave) = 0;
};
//---------------------------------------------------------------------------
class HandleSearchSTG3Task : public MyAbstructTransfer
{
public:
	HandleSearchSTG3Task();
    virtual ~HandleSearchSTG3Task();
    AnsiString What() const;
private:
	VInt8 slaves_;
    virtual void Perform(); 						VIRTUAL_FINAL_
	virtual bool HandleBeginTransfer();				VIRTUAL_FINAL_
    virtual bool HandleEndTransfer();				VIRTUAL_FINAL_
};
//---------------------------------------------------------------------------
class HandleSTG3MeasurementsTask : public HandleSTG3Task
{
public:
	HandleSTG3MeasurementsTask(unsigned slave);
	virtual ~HandleSTG3MeasurementsTask(){}
    AnsiString What() const;
private:
    const unsigned slave_;
    bool hasGot_;
    double conc_;
    bool porog1_;
    bool porog2_;

    virtual void PerformSTG3(); 					VIRTUAL_FINAL_
	virtual bool HandleBeginTransfer();				VIRTUAL_FINAL_
    virtual bool HandleEndTransfer();				VIRTUAL_FINAL_
    virtual bool HandleNotAnswer(unsigned slave);	VIRTUAL_FINAL_
};
//---------------------------------------------------------------------------
class HandleSTG3WriteTask : public MyAbstructTransfer
{
public:
	HandleSTG3WriteTask(unsigned slave, unsigned paramIndex, double val);
	virtual ~HandleSTG3WriteTask(){}
    AnsiString What() const;
private:
    const unsigned slave_, paramIndex_, writeCmdCode_;
    const double val_;
    bool writen_;
    
    virtual void Perform(); 					VIRTUAL_FINAL_
	virtual bool HandleBeginTransfer();				VIRTUAL_FINAL_
    virtual bool HandleEndTransfer();				VIRTUAL_FINAL_
    LRESULT ShowErrorMessage(const AnsiString& msg);
};
//---------------------------------------------------------------------------
class HandleSTG3SetSlaveTask : public MyAbstructTransfer
{
public:
	explicit HandleSTG3SetSlaveTask(unsigned slave);
    virtual ~HandleSTG3SetSlaveTask(){}
    virtual AnsiString What() const;
private:
	const unsigned slave_;
    virtual void Perform();							VIRTUAL_FINAL_
    virtual bool HandleBeginTransfer();				VIRTUAL_FINAL_
    virtual bool HandleEndTransfer();				VIRTUAL_FINAL_
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class HandleSTG3ReadModbustTableTask : public HandleSTG3Task
{
public:
	HandleSTG3ReadModbustTableTask(unsigned slave);
	virtual ~HandleSTG3ReadModbustTableTask(){}
    AnsiString What() const;
private:
    const unsigned slave_;
    bool hasGot_;
    double dt_[STG3::PRMS_COUNT];

    virtual void PerformSTG3(); 					VIRTUAL_FINAL_
	virtual bool HandleBeginTransfer();				VIRTUAL_FINAL_
    virtual bool HandleEndTransfer();				VIRTUAL_FINAL_
    virtual bool HandleNotAnswer(unsigned slave);	VIRTUAL_FINAL_
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

