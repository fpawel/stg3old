//---------------------------------------------------------------------------
#include <vcl.h>
#include <cassert>
#pragma hdrstop

#include "Unit1.h"
#include "appcfg.h"
#include "MyProcNfo.hpp"
#include "MyModFN.hpp"
#include "FreeAndNuLL.hpp"
#include "MyIostream.h"
#include "guicon.h"
#include "MyWindow.h"
#include "AnsiStringUtils_.h"
#include "MyFileCtrl.h"
#include "MyExeption.h"
#include "TransferManage.h"
//------------------------------------------------------------------------------
USEFORM("Unit1.cpp", Form1);
USEFORM("ComPort\Dlg\uFormComPortSetsDlg.cpp", FormComPortSetsDlg);
USEFORM("Dialogs\uFrmWait.cpp", FrmWait);
//---------------------------------------------------------------------------
void RunVCLApplication()
{
	try
	{
		Application->Initialize();
		Application->Title = THIS_APPLICATION_TITLE;
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
}
//------------------------------------------------------------------------------

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ��������� �������������� ���������� ����������
	if( !IsUniqueAppInstance() )
    {
    	MyMessageBox(NULL,	"���������� "THIS_APPLICATION_TITLE"\n ��� ��������.",
        THIS_APPLICATION_TITLE,	MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_OK, THIS_APPLICATION_TITLE   );
        return 0;
    }

    // ������������� ������� �������
    GUIConT& guiCon = GUIConsole::Instance();
    guiCon.Create();

    try
    {
        // ���������������� �������� ����������
		TransferManagerT& tmngr = TransferManager();


    	// ����� ����� � VCL!!!
		RunVCLApplication();

    	//���������� �����
        tmngr.StopThread();

    	// ������� Form1
    	FreeAndNULL(Form1);

    	// ������� ��������� ����������
        delete &tmngr;

    }
    catch ( ... )
    {
    	const PMyExcpt excpt = RethrowMyException(GET_FLINE_);
        MyWCout("����������! ����� �� ����������!\n"+excpt->ToStringAll()+"\n" );
        MyMessageBox(NULL, excpt->Message().c_str(), THIS_APPLICATION_TITLE,
        	MB_OK | MB_ICONERROR | MB_SYSTEMMODAL, THIS_APPLICATION_TITLE   );
    }

    // ��������� ���������� ������� � �������
    guiCon.Backup();

    // ������� �������
    ::FreeConsole();

	return 0;
}
//---------------------------------------------------------------------------
