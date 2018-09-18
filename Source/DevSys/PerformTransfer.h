#ifndef MY_PERFORM_MODBUS_TRANSFER_HEADER_INCLUDED__________
#define MY_PERFORM_MODBUS_TRANSFER_HEADER_INCLUDED__________

#include "..\my_include\free_types_.h"

/**
*		������ ������� ��������� �������� �������������.
*		������������ � ����� "hromat_sys.h".
*		�������� 100% ������������ ��������.
*/
unsigned PerformModbusTransfer
(
	uInt8 slave,
	uInt8 commandCode,			// ��� ��������
	ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
);

#endif
