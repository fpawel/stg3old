//---------------------------------------------------------------------------
#ifndef myThreadH
#define myThreadH

//---------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"
//---------------------------------------------------------------------------

//vcl
#include <Classes.hpp>

#define USE_MY_THREAD_TERMINATE__

namespace my
{
	typedef void __fastcall (__closure *SynchronizeMethodT)(TThreadMethod);
    typedef void (__closure *ThreadProcedureT )( void );

	class Thread : public boost::noncopyable
    {   
    	class Impl;
    	boost::shared_ptr <Impl> impl_;

    public:
    	explicit Thread( ThreadProcedureT, TThreadPriority priority = tpNormal);
        ~Thread();

        void SetCallBack(ThreadProcedureT);

    	void Run();
        void Stop();

        bool WasStoped() const;
        bool IsTerminated() const;

        void SetPriority(TThreadPriority);
        TThreadPriority GetPriority() const;
        
        #ifdef USE_MY_THREAD_TERMINATE__
        #pragma message "Attention! Method my::Thread::Terminate() is dangerous!"
        void Terminate();
        #else
        #pragma message "Its ok to disable Method my::Thread::Terminate() cause it is dangerous."
        #endif
    };


};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
