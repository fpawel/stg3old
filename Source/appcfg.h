#ifndef THIS_APPLICATION_CONFIGURATION_TOOLS_HEADER_INCLUDED__________
#define THIS_APPLICATION_CONFIGURATION_TOOLS_HEADER_INCLUDED__________

/**
*		����� ������������� ������ �� ����� � ����������� ����������
*/
#include "IniFileMacro.h"
#include <inifiles.hpp>
#include "MyModFN.hpp"
#include "..\my_include\free_types_.h"
//------------------------------------------------------------------------------
#define THIS_APPLICATION_TITLE "���-3"
//------------------------------------------------------------------------------

DEFINE_NONDELETEABLE_INIFILE_EX_( Form1, "form1.ini" )
DEFINE_NONDELETEABLE_INIFILE_EX_( Config, "config.ini" )
DEFINE_NONDELETEABLE_INIFILE_EX_( Comm, "comm.ini" )
DEFINE_NONDELETEABLE_INIFILE_EX_( Devices, "devices.cfg" )

#define COMPORT_SEKT "COMPORT"
#define TRANSFERS_SEKT "��������"
#define COMPORT_IDX_ID "����"
#define WRITE_DELAY_ID "��������_������_����"
#define TIME_OUT_ID "�������_��������_������_����"
#define RXD_FRAME_SILENT_TIME "������������_��������_�������_�����_����"
#define MUST_I_LOG_COMPORT_DATA "������"
#define TRANSFERS_THREAD_PRIORITY_ID "���������_�������_������"

/**
*		�������� ������ � ��������� ��������� ����������
*/
namespace RPT_TRNSFR_CNTXT{ enum{
	BEG,        // ����� ��������� ���������
    END,        // ��������� ��������� ���������
    ITFC,       // ���������� ��������� ������ ����� ( �����������/�������� ���������� )
    ERR,        // ������
    START_THREAD,
    STOP_THREAD,
    COUNT
}; };

namespace STG3
{
	enum { SHK_0, SHK_K, LIM1, LIM2, PRMS_COUNT };
	const ConstCString PRM_NAME[PRMS_COUNT] = { "���1", "���2", "�����1", "�����2"	};
	const unsigned WRITE_CMD_CODE[PRMS_COUNT] =	{ 0x0000, 0x0100, 0x1000, 0x1100 };

    const unsigned MODBUS_REG[PRMS_COUNT] =	{ 256, 258, 2, 4 };

    //enum { SET_SLAVE_CODE = 0x03 };
};




#endif