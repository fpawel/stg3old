//---------------------------------------------------------------------------


#pragma hdrstop

#include "TaskList.h"

//vcl
#include <dialogs.hpp>

//my
#include "..\my_include\MyDynamicBitset.h"

//------------------------------------------------------------------------------

#pragma package(smart_init)

//------------------------------------------------------------------------------
TaskList::TaskList() : mainFormThreadId_( GetCurrentThreadId() )
{}
//------------------------------------------------------------------------------
void TaskList::DebugEdit()
{
	// получить итераторы списка
    std::vector<IterT> itrs;
    {
    IterT i = body_.begin();
    for( ++i; i!=body_.end(); ++i ) itrs.push_back(i);
    }
    const unsigned sz = itrs.size();
    if( sz==0 ) return;
    AnsiString s;
    for( unsigned i = 0; i<sz; ++i )
    	s = s + AnsiString().sprintf("%d. %s\n", i+1, (*itrs[i])->What() );
    AnsiString num;
    if( !InputQuery("", s, num) ) return;
    boost::dynamic_bitset<> bs = my::StrFToBitset( num.c_str() );
    AnsiString sss = my::BitsetToStrF(bs).c_str();
    for( unsigned i=0; i<sz; ++i) if( my::GetBsBit(i+1, bs ) ) body_.erase(itrs[i]);
}
//------------------------------------------------------------------------------
void TaskList::PushBackTail( PTransfer task )
{
    	assert( mainFormThreadId_== GetCurrentThreadId() );
    	tail_.push_back( task );
}
//------------------------------------------------------------------------------
void TaskList::PushFrontHead( PTransfer task )
{
    	assert( mainFormThreadId_== GetCurrentThreadId() );
    	head_.push_front( task );
}
//------------------------------------------------------------------------------
void TaskList::PopFrontBody()
{
    	assert( mainFormThreadId_!=GetCurrentThreadId() || InSendMessage()==TRUE   );
        body_.pop_front();
}
//------------------------------------------------------------------------------
void TaskList::PushBackBody( PTransfer task )
{
    	assert( mainFormThreadId_!=GetCurrentThreadId() || InSendMessage()==TRUE   );
    	body_.push_back( task );
}
//------------------------------------------------------------------------------
PTransfer TaskList::FrontBody() const
{
    	assert( mainFormThreadId_!=GetCurrentThreadId() || InSendMessage()==TRUE   );
    	return body_.empty() ? PTransfer() : body_.front();
}
//------------------------------------------------------------------------------
LRESULT TaskList::Merge()
{
    	assert( InSendMessage()==TRUE );
    	body_.insert( body_.begin(), head_.begin(), head_.end() );
        head_.clear();
        while( !tail_.empty() )
        {
        	body_.push_back( tail_.front() );
        	tail_.pop_front();
        }
	return 0;
}
//------------------------------------------------------------------------------
void TaskList::Clear()
{
	assert( mainFormThreadId_==GetCurrentThreadId()  );
    body_.clear();
    head_.clear();
    tail_.clear();
}
