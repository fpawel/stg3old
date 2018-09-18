//---------------------------------------------------------------------------
#ifndef guiconH
#define guiconH
//---------------------------------------------------------------------------
#include "Loki\singleton.h"
#include "MyIostream.h"

// windows
#include <basetsd.h>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"


//------------------------------------------------------------------------------
void MyWriteConsole(const wchar_t* str, DWORD nNumberOfCharsToWrite);
//------------------------------------------------------------------------------
LRESULT MyWCout(const AnsiString& str);


class MyConQuote : public boost::noncopyable
{
public:
	MyConQuote( const AnsiString& s ) :  s_(s)
    {
    	MyWCout( "<" + s_ + ">\n");

    }
    ~MyConQuote()
    {
    	MyWCout( "<\\" + s_ + ">\n");
    }
private:
	const AnsiString s_;

};
#define LOG_MY_CON_QUOTES(s) const MyConQuote qteCout( s )
//------------------------------------------------------------------------------
class GUIConT : public boost::noncopyable
{
public:
    explicit GUIConT();
    void InitializePaths();
    void Create();
    void Redirect();
    bool IsCreated() const { return isCreated_; }
    bool IsRedirected() const  { return isRedirected_; }
    void Backup();
private:
    bool isCreated_;
    bool isRedirected_;
    AnsiString logDir_, logFn_, logArchiveFileName_;
};
//------------------------------------------------------------------------------
typedef Loki::SingletonHolder
    <
        GUIConT,
        Loki::CreateUsingNew,
    	Loki::NoDestroy
    > GUIConsole;
//------------------------------------------------------------------------------
void SaveConsoleToFile(const AnsiString& fn);
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
