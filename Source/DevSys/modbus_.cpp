#include <DateUtils.hpp>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "modbus_.h"
#include "bcd_.h"
#include "my_array_size.h"
#include "Loki\static_check.h"
#include "MyExeption.h"
// для вызова функции PerformModbusTransfer
#include "PerformTransfer.h"
#include "MySynchronize.h"
#include "AnsiStringUtils_.h"


// предварительные объявления классов-исключений для ХРОМАТ
// #include "HromatExceptLst.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
enum{ READ_MODBUS_FILE_CMD_CODE = 0x41 };
enum{ WRITE_MODBUS_FILE_CMD_CODE = 0x51 };



namespace PrivateHromatModbus
{

	typedef struct{ uInt8 aa[3]; } AStruct;
    struct StaticCheckThisModule
    {
        void Check()
        {
        	//LOKI_STATIC_CHECK(sizeof(AStruct)==3, MSG);
            //STATIC_CHECK( sizeof(ModbusFileInfo)==MODBUS_FILE_INFO_SIZE+1, ModbusFileInfoSizeMismatch);
        }
    };
};
//  Исключение "MODBUS"
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbus, ExternalException );
//  Исключение комманды 16
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusCmd10h, HromatModbusException );
//  Исключение комманды 3
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusCmd3h, HromatModbusException );
//  Исключение комманды 0x41
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusCmd41h, HromatModbusException );
//  Исключение для информации о несовпадении запрошенного объёма информации с полученным
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusGotAndRequestedLengthMismatch,
	HromatModbusException );

//  Исключение для информации об ошибке записи файла
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusFileWrite, HromatModbusException );

//  Исключение для информации об ошибке преобразования BCD
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusBCDToUnsignedIntConversion,
	HromatModbusException );
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusBCDToFloatConversion,
	HromatModbusException );


#undef MY_THROW_
#undef MY_THROW_CLASS_
#define THROW_MODBUS_EXCEPTION_(THECLASS, MSG)\
	throw MyExcptionFactory::Create<MyHromatModbus##THECLASS##Exception>(GET_FLINE_, MSG)

//------------------------------------------------------------------------------
void WriteModbus4Bytes( const uInt8 slave, uInt16 cmd, const uInt8* dt )
{

	 my::SynchronizeWCout( MYSPRINTF_( "Отправка команды %s(%s, %s, %s, %s)\n",
        	IntToHex(cmd,2),
        	IntToHex(dt[0],2),	IntToHex(dt[1],2),
        	IntToHex(dt[2],2),	IntToHex(dt[3],2) ));
	enum
    {
    	MODUS_WRITE_REGISTER_COMMAND_CODE = 16,
        MODUS_WRITE_REGISTER_ADDR = 32,
        MODUS_WRITE_REGISTERS_COUNT = 3,
        MODUS_WRITE_DATA_LEN = 6,
        MODUS_WRITE_STR_LEN = 11
    };

    ByteBuffPtr rxd, rxdEnd;

	uInt8 txd[MODUS_WRITE_STR_LEN];
    txd[0] = MODUS_WRITE_REGISTER_ADDR >> 8;
    txd[1] = MODUS_WRITE_REGISTER_ADDR;
    txd[2] = MODUS_WRITE_REGISTERS_COUNT>>8;
    txd[3] = MODUS_WRITE_REGISTERS_COUNT;
    txd[4] = MODUS_WRITE_DATA_LEN;
    // данные
    txd[5] = cmd >> 8;
    txd[6] = cmd;
    std::copy( dt, dt+4, txd+7 );

    PerformModbusTransfer(slave, MODUS_WRITE_REGISTER_COMMAND_CODE, txd,
    	txd+MODUS_WRITE_STR_LEN, rxd, rxdEnd);

    if(slave==0) return;

    if( rxdEnd - rxd !=4 )
        THROW_MODBUS_EXCEPTION_(Cmd10h, "CMD16: rxdEnd-rxd!=4");
    if( !std::equal(txd, txd+4, rxd) )
        THROW_MODBUS_EXCEPTION_(Cmd10h, "CMD16: !std::equal(txd, txd+4, rxd)");
}
//------------------------------------------------------------------------------
void WriteModbusFloat( const uInt8 slave, uInt16 cmd, double val )
{
	my::SynchronizeWCout( MYSPRINTF_( "Отправка команды %s(%s)\n", IntToHex(cmd,2), FloatBCDToStr(val) ));
	uInt8 bcd[4];
    FloatToBCD(val, bcd );
    WriteModbus4Bytes( slave, cmd, bcd);
}
//------------------------------------------------------------------------------
void ReadModbusRegistersData( const uInt8 slave,  uInt16 regAddr,
	unsigned regCount,	ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd )
{
	enum { MODUS_READ_REGISTER_COMMAND_CODE = 0x03 };

    const unsigned waitStrLen = regCount*2;
    assert( regCount<0xFF );

	uInt8 txd[4];
    txd[0] = regAddr >> 8;
    txd[1] = regAddr;
    txd[2] = regCount>>8;
    txd[3] = regCount;
    
    PerformModbusTransfer(slave, MODUS_READ_REGISTER_COMMAND_CODE, txd, txd+4, rxd, rxdEnd);

    const unsigned rxdLen = rxdEnd - rxd;

    if( rxdLen!=waitStrLen + 1)
        THROW_MODBUS_EXCEPTION_(Cmd3h, "CMD3 rxdLen!=waitStrLen+1");
    if( waitStrLen != static_cast<unsigned>(*rxd) )
        THROW_MODBUS_EXCEPTION_(Cmd3h, "CMD3 waitStrLen!=rxd[0]");
    ++rxd;
}
//------------------------------------------------------------------------------
void ReadModbusFloat(  const uInt8 slave, uInt16 regAddr, unsigned count, double *pResult )
{
	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( slave, regAddr, count*2, rxd, rxdEnd );
	TransformBCDDataToFloat( rxd, rxdEnd, pResult );
}
//------------------------------------------------------------------------------
unsigned ReadModbusUnsigned( const uInt8 slave, uInt16 regAddr )
{
	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( slave, regAddr, 2, rxd, rxdEnd );
    // 10000111 = 0x87
    if( (rxd[0] & 0x78)!=0  )
    	THROW_MODBUS_EXCEPTION_(BCDToUnsignedIntConversion,
        AnsiString().sprintf("Invalid BCD unsigned [%d,%d,%d,%d] at reg %d",
        rxd[0], rxd[1], rxd[2], rxd[3], regAddr).c_str() );
    return Ceil( BCDToFloat(rxd).Value );
}
//------------------------------------------------------------------------------
int ReadModbusSigned(  const uInt8 slave, uInt16 regAddr )
{
	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( slave, regAddr, 2, rxd, rxdEnd );
    // 10000111 = 0x87
    if( (rxd[0] & 0xF8)!=0  )
    	THROW_MODBUS_EXCEPTION_(BCDToUnsignedIntConversion,
        AnsiString().sprintf("Invalid BCD integer [%d,%d,%d,%d] at reg %d",
        rxd[0], rxd[1], rxd[2], rxd[3], regAddr).c_str() );
    return Ceil( BCDToFloat(rxd).Value );
}
//------------------------------------------------------------------------------

