//------------------------------------------------------------------------------
#ifndef MY_ABSTRUCT_COMMUNICATION_PORT_HEADER_INCLUDED__________
#define MY_ABSTRUCT_COMMUNICATION_PORT_HEADER_INCLUDED__________
//------------------------------------------------------------------------------
#include <system.hpp>
#include "boost\noncopyable.hpp"
//------------------------------------------------------------------------------
    
	// интерфейс для канала приёмопередачи
	class MyPort : public boost::noncopyable
	{
    typedef void (__closure *TNotifyChange)(unsigned, const MyPort&, const void*, int);
	public:
        enum {stConnect, stDisconnect, stWrite, stRead};

        explicit MyPort() : isConnected_(0), notifyChange_(NULL)
        {}

        virtual ~MyPort() {}

		virtual AnsiString Description() const = 0;
        
        virtual bool ConnectionWasBreaked() const { return false; }

        void Connect()
        {
            Open();
            isConnected_ = 1;
            if(notifyChange_) notifyChange_(stConnect, *this, NULL, 0);
        }
        void Disconnect()
        {
            Close();
            isConnected_ = 0;
            if(notifyChange_) notifyChange_(stDisconnect, *this, NULL, 0);
        }
        bool IsConnected() const { return isConnected_; }
        void SetNotifyChange(TNotifyChange notifyChange) { notifyChange_ = notifyChange; }

        virtual int Write(void*, int) = 0;
		virtual int Read(void*, int) = 0;
		virtual int GetRxDSize() = 0;
		virtual void Purge() = 0;
    private:
        bool isConnected_;
        TNotifyChange notifyChange_;
        virtual void Open() = 0;
        virtual void Close() = 0;
	};



#endif
