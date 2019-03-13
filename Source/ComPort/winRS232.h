//---------------------------------------------------------------------------
#ifndef winRS232H
#define winRS232H
//---------------------------------------------------------------------------
//#include <windows.h>
//---------------------------------------------------------------------------
// vcl
#include <system.hpp>

// std
#include <string>
#include <vector>
//---------------------------------------------------------------------------
//#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"
//---------------------------------------------------------------------------
#include "CommSets.h"
#include "..\my_include\myport.hpp"

//---------------------------------------------------------------------------
namespace my
{
namespace RS232
{
	// �������� �������� ������ �����-������ ���-�����
	AnsiString ErrorsMaskToStr( DWORD commErrorsMask );
    
	// ��������� �� ����� ����������� ������ COM-�����
	bool HasMaskCriticalError( DWORD commErrorsMask );

    // �������� ���������� �����
    void* CreatePortHandle(unsigned idx);

    void SetConfig( void *hComm, const COMMCONFIG& commConfig );
    COMMCONFIG GetConfig( void *hComm  );

    COMMPROP GetProperties(void* hComm);
    void SetTxDRxDBuffsSizes( void* hComm, DWORD txdSize, DWORD rxdSize );

    //--------------------------------------------------------------------------
    // ����� "���-����"
    //--------------------------------------------------------------------------
    class Port : public MyPort
    {

    public:
    	explicit Port();
        virtual ~Port();
        //----------------------------------------------------------------------
        // ���������� ������
        //----------------------------------------------------------------------
        // �������� ���������� ������ � ��������.
    	// ���� ��������� ������ �����-��������, ��������� ���������� CommException
    	virtual int GetRxDSize();
    	virtual void Purge();
        virtual int Write( void* pBuff, int count );
    	virtual int Read(  void* pBuff, int count );
        virtual AnsiString Description() const;

        void EscapeComm( DWORD dwFunc );

    	const Setings& GetSettings() const;
        void SetSettings(const Setings& sets);
        void SetSettingsDefault();

        AnsiString GetPortName() const;
        void SetPortName(const AnsiString&);

        void SetDCB(const DCB& dcb);
        void SetTimeOuts( const COMMTIMEOUTS& timeOuts );
        void SetBuffsSizes(DWORD txdBuffSize, DWORD rxdBuffSize);

        COMMTIMEOUTS ReciveTimeOuts();
        DCB ReciveDCB();

        	// �������� ������ ��� ��������� ������������ �����
        	// hwnd - ����, ��� �������� ������ ������ ���������
        	// getFromSystem: ���� false, ���������������� ������
        	// �������� ����������� �����, ����� �����������,
        	// ����������� � ������� GetCommConfig
        void SetupDialog(HWND hwnd, bool getFromSystem );

        bool IsHandleCreated() const;



    private:
        void* hComm_;
        //unsigned portIdx_;
        AnsiString portName_;
        // ��������� �����
        Setings sets_;

        bool mustApplyDCB_, mustApplyTimeOuts_, mustApplyBuffsSizes_;

        struct OpenTemporaryHelper
        {
        	void*& hComm_;
            const bool wasOpened_;
        	OpenTemporaryHelper(void*& hComm, const AnsiString& portName);
            ~OpenTemporaryHelper();
        };
        void ApplyBuffsSizes();
        void ApplyDCB();
        void ApplyTimeOuts();

        virtual void Open();
        virtual void Close();


    };// ����� "���-����"
    //--------------------------------------------------------------------------
};// namespace Comm
};//namespace my

#endif
