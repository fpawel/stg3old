//---------------------------------------------------------------------------
#ifndef AnsiStringUtils_H
#define AnsiStringUtils_H
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <utility>


#define MYSPRINTF_ AnsiString().sprintf

// translates a specified number of characters in a string into the OEM-defined character set
AnsiString MyStringToOem( const AnsiString& );


AnsiString BoolArrayToStr( const bool* b, const bool* e );
void StrToBoolArray( bool* b, bool* e, const AnsiString& s );

typedef std::pair<double,bool> MyDouble;
MyDouble MyStrToD( const AnsiString& s );
bool MyTryStrToFloat( const AnsiString& s, double* pVal = NULL  );

typedef std::pair<long,bool> MyInt;
MyInt MyStrToInt( const AnsiString& s, int radix = 10 );

AnsiString MyFormatFloat( double v, int n );
AnsiString MyDToStr( const MyDouble& v, int n );

void ShowUserInputError(const AnsiString& fld, const AnsiString& val);

AnsiString MyBuffToStr( const unsigned char *buffer, const unsigned char *bufferEnd,
    const AnsiString& ss = "" );

AnsiString MakeFileNameAsDate( const AnsiString& dir, const AnsiString& ext,
    TDateTime date, const AnsiString& add_back = "" );

AnsiString DateToPath( TDateTime date );
AnsiString DateToFilename( TDateTime date );

bool IsAlphaNumber( const AnsiString& s );

AnsiString NormString( const AnsiString& s )
{
    return s.IsEmpty() ? AnsiString(" ") : s;
}

unsigned CountAfterCommaDigits(double v);

AnsiString MyDateTimeToStr(TDateTime);
AnsiString MyFormatMonth( unsigned month );







//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
