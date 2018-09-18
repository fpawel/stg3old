//---------------------------------------------------------------------------
#include <math>
//---------------------------------------------------------------------------
#pragma hdrstop

#include "bcd_.h"
#include "AnsiStringUtils_.h"
#include "MyExeption.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//�������������� �������-����������� ������������ ����� � ������������
/*
double BCDToFloat_Throw(const unsigned char* bcd)
{
	for(int i=1;i<4;++i)
    	if ( (bcd[i] & 0xF0)>0x90 || (bcd[i] & 0x0F)>0x09 )
        MY_THROW_( AnsiString().sprintf("Error convert bcd[%d,%d,%d,%d] "
        "to float: bad number!", bcd[0], bcd[1], bcd[2], bcd[3]).c_str() );

	AnsiString s = IntToHex(bcd[1],2)+IntToHex(bcd[2],2)+IntToHex(bcd[3],2);
    int zap = bcd[0] & 0x07;
    try
    {
    	if (zap>0) s.Insert(',', 6-zap+1 );
    	if ( (bcd[0] & 0x80)!=0 ) s.Insert('-',1);
     	return StrToFloat( s );
    }
    catch(...)
    {
    	assert(!"������ ��� �������������� BCD � double!");
        return 0;
    }
}
*/
//---------------------------------------------------------------------------
// ������������ �� ������ � �������-����������
bool IsBCDFloat( const AnsiString& s )
{
	const MyDouble val = MyStrToD( s );
    return val.second && fabs(val.first)< MAX_BCD_FLOAT;
}
//---------------------------------------------------------------------------
//�������������� �������-����������� ������������ ����� � ������������
BCDValue BCDToFloat(const unsigned char* bcd)
{
    BCDValue x;

	for(int i=1;i<4;++i)
    	if ( (bcd[i] & 0xF0)>0x90 || (bcd[i] & 0x0F)>0x09 )
        {
            x.Value = MAX_BCD_FLOAT;
        	return x;
        }
	AnsiString s = IntToHex(bcd[1],2)+IntToHex(bcd[2],2)+IntToHex(bcd[3],2);

    int zap = bcd[0] & 0x07;
    if (zap)
    	s.Insert(',', 6-zap+1 );


    x.Value = StrToFloat( s );
    if (bcd[0] & 0x80)
    	x.Value *= -1;

    x.Porog1 = bcd[0] & 0x08;
    x.Porog2 = bcd[0] & 0x10;

    return x;
}
//---------------------------------------------------------------------------
// ������������� ������ � �����, ���� ����������, ������� MAX_BCD_FLOAT
double StrToFloatBCD( const AnsiString& s )
{
	const MyDouble val = MyStrToD( s );
    return val.second ? val.first : MAX_BCD_FLOAT;
}
//---------------------------------------------------------------------------
// ������������� ����� � ������, ���� ����� �� ������ >= MAX_BCD_FLOAT, ������� ""
AnsiString FloatBCDToStr( double val )
{
    return fabs(val)>=MAX_BCD_FLOAT ? AnsiString(" ") : FormatFloat("#0.######",val);
}
//---------------------------------------------------------------------------
// ��������� ����� � BCD
void FloatToBCD(double val, unsigned char* res)
{
    if (fabs(val)>=MAX_BCD_FLOAT)
    {
    	res[0] = res[1] = res[2] = res[3] = 0xFF;
        return;
    }


	double absVal = val;
    res[0] = 0;
    if (val<0)
    {
    	res[0] = 0x80;
        absVal *= -1;
    };


    int zap = 0;
    if (val!=0)
    {
    	int n = FormatFloat(".000000",absVal).Pos(',');
    	zap = 7-n;   //������� �������
    	if (zap<0) zap = 0;
        else
        	if (zap>6) zap = 6;
    }
    absVal *= pow(10,zap);
    const AnsiString s = FormatFloat("000000",absVal);
    res[0] += zap;
    res[1] = StrToInt(s[1])*16+StrToInt(s[2]);
    res[2] = StrToInt(s[3])*16+StrToInt(s[4]);
    res[3] = StrToInt(s[5])*16+StrToInt(s[6]);
}
//---------------------------------------------------------------------------
std::vector<unsigned char> FloatToBCD(double val)
{
    std::vector<unsigned char> ret(4,0);
    FloatToBCD(val, &ret.at(0) );
    return ret;
}
