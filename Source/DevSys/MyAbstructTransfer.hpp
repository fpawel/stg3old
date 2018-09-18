#ifndef MY_ABSTRUCT_TRANSFER_HEADER_INCLUDED_____
#define MY_ABSTRUCT_TRANSFER_HEADER_INCLUDED_____

//std
#include <utility>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

//vcl
#include <system.hpp>

//my
#include "..\my_include\free_types_.h"
//------------------------------------------------------------------------------
class MyAbstructTransfer : public boost::noncopyable
{
public:
	// ��������� ������� ������ ���������. ���������� - ���������/�� ���������
	virtual bool HandleBeginTransfer() = 0;
    // ��������� ������� ��������� ���������. ���������� - ���������/�� ���������
	virtual bool HandleEndTransfer() = 0;
	// ���������. ���������� - ��������/�� ��������
    virtual void Perform() = 0;
	virtual AnsiString What() const { return " "; }
    virtual ~MyAbstructTransfer() {}
};
//------------------------------------------------------------------------------
typedef boost::shared_ptr<MyAbstructTransfer> PTransfer;

// ������� ��� ��������� ������� ���������
template<class F>
class SingleBackgroundTask : public MyAbstructTransfer
{
public:
	SingleBackgroundTask( F mthd, const AnsiString what ) :
    	mthd_(mthd), what_(what)
    {}
    virtual ~SingleBackgroundTask() {}

private:
    virtual bool HandleBeginTransfer() { mthd_(); return false; }
    virtual bool HandleEndTransfer() { return false; }
    virtual void Perform(){}
    virtual AnsiString What() const  { return what_; }
    const F mthd_;
    const AnsiString what_;
};

template<class F>
MyAbstructTransfer* CreateSingleBackgroundTask( F mthd, const AnsiString what = "" )
{
	return new SingleBackgroundTask<F>(mthd, what);
}





#endif