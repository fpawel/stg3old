//---------------------------------------------------------------------------
#ifndef TaskListH
#define TaskListH
//---------------------------------------------------------------------------

//std
#include <vector>
#include <list>
#include <algorithm>


// my
#include "MyAbstructTransfer.hpp"

// windows
#include <basetsd.h>

template<class T> struct CheckPTransferClass
{
    bool operator()(const PTransfer& task) const
    {
        return dynamic_cast<T*>( task.get() )!=NULL;
    }
};

class TaskList : public boost::noncopyable
{
public:
	explicit TaskList();
    void PushBackTail( PTransfer task );
    void PushFrontHead( PTransfer task );
    void PopFrontBody();
    void PushBackBody( PTransfer task );
    PTransfer FrontBody() const;
    LRESULT Merge();
    void DebugEdit();

    void Print() const;

    void Clear();

    template<class T> void EraseTasksOfClass()
    {
        const CheckPTransferClass<T> pred;
        #define ERASE_IN_LIST_(nn) nn.erase( std::remove_if(nn.begin(), nn.end(), pred ), nn.end() )
        ERASE_IN_LIST_(body_);
        ERASE_IN_LIST_(head_);
        ERASE_IN_LIST_(tail_);
        #undef ERASE_IN_LIST_(nn)
    }

    template<class T> unsigned CountTasksOfClass() const
    {
        const CheckPTransferClass<T> pred;
        unsigned ret = 0;

        #define INC_RET_WIRH_LIST_(nn) ret += std::count_if(nn.begin(), nn.end(), pred )
        INC_RET_WIRH_LIST_( body_);
        INC_RET_WIRH_LIST_( head_);
        INC_RET_WIRH_LIST_( tail_);
        #undef INC_RET_WIRH_LIST_(nn)
        return ret;
    }

    const bool IsEmpty() const
    {
    	return body_.empty() &&  head_.empty() && tail_.empty();
    }

private:
	typedef std::list<PTransfer> ListT;
    typedef ListT::iterator IterT;

    ListT body_, head_, tail_;
    const DWORD mainFormThreadId_;
};





#endif
//---------------------------------------------------------------------------
