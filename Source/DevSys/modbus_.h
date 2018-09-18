//---------------------------------------------------------------------------
#ifndef modbus_H
#define modbus_H
//---------------------------------------------------------------------------
#include "..\my_include\free_types_.h"

/**
*		Базовый класс для исключений модуля
*/
class MyAP430ModbusException;




/**
*		Записать 4 байта
*		throw PMyAP430ModbusException
*/
void WriteModbus4Bytes(  uInt8 slave, uInt16 cmd, const uInt8* dt );
/**
*		Записать число
*/
void WriteModbusFloat(  uInt8 slave, uInt16 cmd, double val );

/**
*		Считать массив
*		throw PMyAP430ModbusException
*/
void ReadModbusRegistersData(  uInt8 slave, uInt16 regAddr,
	unsigned regCount,	ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd );

/**
*		Считать массив вщественных чисел
*		throw PMyAP430ModbusException
*/
void ReadModbusFloat(  uInt8 slave, uInt16 regAddr, unsigned count,	double *pResult);
unsigned ReadModbusUnsigned(  uInt8 slave, uInt16 regAddr );
int ReadModbusSigned(  uInt8 slave, uInt16 regAddr );

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
