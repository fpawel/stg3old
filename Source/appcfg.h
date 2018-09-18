#ifndef THIS_APPLICATION_CONFIGURATION_TOOLS_HEADER_INCLUDED__________
#define THIS_APPLICATION_CONFIGURATION_TOOLS_HEADER_INCLUDED__________

/**
*		Здесь декларированы ссылки на файлы с настройками приложения
*/
#include "IniFileMacro.h"
#include <inifiles.hpp>
#include "MyModFN.hpp"
#include "..\my_include\free_types_.h"
//------------------------------------------------------------------------------
#define THIS_APPLICATION_TITLE "СТГ-3"
//------------------------------------------------------------------------------

DEFINE_NONDELETEABLE_INIFILE_EX_( Form1, "form1.ini" )
DEFINE_NONDELETEABLE_INIFILE_EX_( Config, "config.ini" )
DEFINE_NONDELETEABLE_INIFILE_EX_( Comm, "comm.ini" )
DEFINE_NONDELETEABLE_INIFILE_EX_( Devices, "devices.cfg" )

#define COMPORT_SEKT "COMPORT"
#define TRANSFERS_SEKT "Трансфер"
#define COMPORT_IDX_ID "Порт"
#define WRITE_DELAY_ID "Задержка_записи_мсек"
#define TIME_OUT_ID "таймаут_ожидания_ответа_мсек"
#define RXD_FRAME_SILENT_TIME "длительность_ожидания_символа_приёма_мсек"
#define MUST_I_LOG_COMPORT_DATA "лагать"
#define TRANSFERS_THREAD_PRIORITY_ID "приоритет_второго_потока"

/**
*		контекст отчёта о состоянии менеджера трансферов
*/
namespace RPT_TRNSFR_CNTXT{ enum{
	BEG,        // старт обработки трансфера
    END,        // окончание обработки трансфера
    ITFC,       // изменилось состояние канала связи ( установлено/оборвано соединение )
    ERR,        // ошибка
    START_THREAD,
    STOP_THREAD,
    COUNT
}; };

namespace STG3
{
	enum { SHK_0, SHK_K, LIM1, LIM2, PRMS_COUNT };
	const ConstCString PRM_NAME[PRMS_COUNT] = { "ПГС1", "ПГС2", "Порог1", "Порог2"	};
	const unsigned WRITE_CMD_CODE[PRMS_COUNT] =	{ 0x0000, 0x0100, 0x1000, 0x1100 };

    const unsigned MODBUS_REG[PRMS_COUNT] =	{ 256, 258, 2, 4 };

    //enum { SET_SLAVE_CODE = 0x03 };
};




#endif