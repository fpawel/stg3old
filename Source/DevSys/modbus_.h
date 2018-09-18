//---------------------------------------------------------------------------
#ifndef modbus_H
#define modbus_H
//---------------------------------------------------------------------------
#include "..\my_include\free_types_.h"

/**
*		������� ����� ��� ���������� ������
*/
class MyAP430ModbusException;




/**
*		�������� 4 �����
*		throw PMyAP430ModbusException
*/
void WriteModbus4Bytes(  uInt8 slave, uInt16 cmd, const uInt8* dt );
/**
*		�������� �����
*/
void WriteModbusFloat(  uInt8 slave, uInt16 cmd, double val );

/**
*		������� ������
*		throw PMyAP430ModbusException
*/
void ReadModbusRegistersData(  uInt8 slave, uInt16 regAddr,
	unsigned regCount,	ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd );

/**
*		������� ������ ����������� �����
*		throw PMyAP430ModbusException
*/
void ReadModbusFloat(  uInt8 slave, uInt16 regAddr, unsigned count,	double *pResult);
unsigned ReadModbusUnsigned(  uInt8 slave, uInt16 regAddr );
int ReadModbusSigned(  uInt8 slave, uInt16 regAddr );

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
