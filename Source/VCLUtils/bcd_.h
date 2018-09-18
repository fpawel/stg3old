//---------------------------------------------------------------------------
#ifndef bcd_H
#define bcd_H
//---------------------------------------------------------------------------

//vcl
#include <SysUtils.hpp>

//std
#include <vector>

#define MAX_BCD_FLOAT 999999

//�������������� �������-����������� ������������ ����� � ������������

 struct BCDValue  {
    double Value ;
    bool Porog1 ;
    bool Porog2 ;
};
BCDValue BCDToFloat(const unsigned char* bcd);
//double BCDToFloat_Throw(const unsigned char* bcd);

void TransformBCDDataToFloat( const unsigned char* bcd,	const unsigned char* bcdEnd, double *out );

// ��������� ����� � BCD
void FloatToBCD(double val, unsigned char* res);
std::vector<unsigned char> FloatToBCD(double val);

// ������������� ������ � �����, ���� ����������, ������� MAX_BCD_FLOAT
double StrToFloatBCD( const AnsiString& s );

// ������������ �� ������ � �������-����������
bool IsBCDFloat( const AnsiString& s );

// ������������� ����� � ������, ���� ����� �� ������ >= MAX_BCD_FLOAT, ������� ""
AnsiString FloatBCDToStr( double val );






#endif
