//---------------------------------------------------------------------------

#ifndef MyExeptionH
#define MyExeptionH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
//---------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"

//std
#include <vector>
    /**
    * ��������� ��� ������������ ���������� � ����� ������������� ����������.
	*/
    typedef std::pair<unsigned, AnsiString> FileLine;


	/**
	* ����� ������� ����� ��� ���� ���������� �������. ��� ���������� ������
	* ������������ � ���� sh_ptr<MyException>, ������� �������� ��������� ��
	* ���������� ����������, ����������� �� MyException. � ��������, ���������
	* ����������� ����� ���������� ����� ��������� ������ � ��� ������, ����� �
	* ������������ ������ ���� ����������� �������� ��� �� "������ MyException", �
	* � ��������� ������� ������ �������� � ��� MyException, ����������: "����������
	* ������ ���� ������ �� ��������� ���������".
	*/
    class MyException;

    /**
	* ����������� ���� ������ ��������� �� ����������
	*/
	typedef boost::shared_ptr<const MyException> ConstPMyExcpt;
    typedef boost::shared_ptr<MyException> PMyExcpt;

	/**
	* ������ ������� �������������� ������� ���������� � ���������� ��������� ��
	* ���� PMyExcpt. ��� ����� ���� ������� ������ �� ����� catch. ���
	* �������� ������� location ������ ������������ ������ _FLINE_.
	* ���� ������������ ��������� ������������� �������������� (�������������)
	* ����������� ��� ��������� ����� ��������� ���������� � �������������
	* ����������� ������������ ����, �� ����� ����� �������� ����������� ����������
	* ������ �������, ������������ ��� �����������.
    */
	PMyExcpt RethrowMyException(const FileLine& location);

    /**
    *   ��������� ������������� MyException
	*   ������������� ��� �������� �������� ������ ������������. ������� ������
	*   � ��������� ������ �� ���������� ����� ����������, ��������� �,
	*   ��������, ���������� ����������. ��� �������� ������� loc ������
	*   ������������ ������ _FLINE_.
	*/
    struct MyExcptionFactory
    {

        template<class E>
        static PMyExcpt Create(const FileLine& loc, const AnsiString& msg, const PMyExcpt nest = PMyExcpt() )
        {
        	return PMyExcpt( new E(loc, msg, nest) );
        }

        template<class E,typename A>
        static PMyExcpt Create(A arg, const FileLine& loc, const AnsiString& msg, const PMyExcpt nest = PMyExcpt() )
        {
        	return PMyExcpt( new E(arg, loc, msg, nest) );
        }
    };

    /**
    *       ���������� MyException
    */
    class MyException : public boost::noncopyable
    {
    friend struct MyExcptionFactory;

    public:

        /** ����������� ����������. */
	    virtual ~MyException(){}

	    /**
        *   ���������� ��� ������-����������. ������ ����������� ���������������� �
	    *   ����������� �������.
        */
	    virtual AnsiString ClassName() const;

        /**
        *       ����� ������������� ����������
        */
	    const FileLine& Location() const ;
        
        /**
	    /*      ���������
        */
	    const AnsiString& Message() const;

	    /**
        *       ��������� �� ��������� ����������, ������� ��������, ��� 0
        */
        const MyException* Nested() const;

        /**
        *       ��������� �� ������� �� ��������� ���������� ��� this
        */
        const MyException* Root() const;

        /**
        *   ���������� "������ ��������" ����������, ���������� ��� ������ � �����
	    *   �������������.
        */
	    AnsiString ToString() const;

	    /**
        *   ���������� "������ ��������" �������� ���������� � ���� ����������,
	    *   ��������� � ����. ����������, ���������� ���������� ToString() �
	    *   ������� �� ���������� ������������ � ���� ������.
        */
        AnsiString ToStringAll() const;
        

        /**
        *       �������������� � �������� ����������� ����
        */
        template<class E>
        const E* const As() const
        { return dynamic_cast<const E* const>(this); }

        template<class E>
        bool TypeOf() const
        { return dynamic_cast<const E* const>(this); }



    protected:
        /**
        *   ���������� �����������. ��� �������� �������� ���������� ������������
	    *   ������� NewMyException().
        */
        MyException(const FileLine& loc, const AnsiString& msg, const PMyExcpt nest );
    private:
        // ����� ������������� ����������
	    const FileLine location_;
	    // ���������
	    const AnsiString message_;
	    // ��������� �� ��������� ����������, ������� ��������, ��� 0
	    const PMyExcpt nested_;
    };

    
    /**
	*		������� ��� ����������� ������� My##THECLASS##Exception,
    *		����������� �� My##THEBASECLASS
	*		� ������������ ���������� �������������
	*		My##THECLASS##Exception(const FileLine& loc, const AnsiString& msg, PMyExcpt nest)
	*/


    #define DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, THENAME )\
    class My##THECLASS##Exception : public My##THEBASECLASS\
    {\
    	friend struct MyExcptionFactory;\
    protected:\
    	My##THECLASS##Exception(const FileLine& loc, const AnsiString& msg, PMyExcpt nest) :\
 		My##THEBASECLASS##(loc, msg, nest){}\
    public:\
    	virtual ~My##THECLASS##Exception(){}\
		virtual AnsiString ClassName() const { return THENAME; }\
    }

    #define DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(THECLASS, THEBASECLASS )\
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, "My"#THECLASS"Exception" )

    /**
	*		MyExternalException
	*		����� ������� ����� ��� ������������ ����������, ���������� �������� ��
	*		��������� � ��������, ����������� MyException.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(External, Exception );
	/**
	*      ���������� ��� ������������ std::exception � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Std, ExternalException );
    /**
	*  ���������� ��� ������������ Exception �� VCL � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(VCL, ExternalException );
    /**
	*  ���������� ��� ������������ Exception �� VCL � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(AccessViolation, ExternalException );
    /**
	*  	���������� ��� �������������� � ������� ����������, ��� �������� �� �������
    *	����������.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Unknown, ExternalException );

    /**
	*  	���������� ��� �������������� �� ������ ������ ������� Win32 API
	*/
    AnsiString WindowsErrorCodeToStr( unsigned long errorCode );
    void ShowLastWindowsError(const AnsiString& msg = "");

	class MyWindowsException : public MyExternalException
    {
    	friend struct MyExcptionFactory;
    	AnsiString MakeMsg(const AnsiString& msg, unsigned long int code );
        unsigned code_;
 	protected:
        MyWindowsException(unsigned long int code, const FileLine& loc,
        const AnsiString& msg, PMyExcpt nest);
        MyWindowsException(const FileLine& loc,
            const AnsiString& msg, PMyExcpt nest);

 	public:
        unsigned Code() const { return code_; }
      	virtual AnsiString ClassName() const;
    };

    AnsiString FormatMyException( const MyException* excpt );
    //------------------------------------------------------------------------------
    AnsiString FormatMyExceptionStack( const MyException* excpt );
    //---------------------------------------------------------------------------

    


    /**
	*		������ ��� ������ ������� Win32 API.
    *		���� ����������� ������ ����� FALSE, ������������ ���������� MyWindowsException.
    *		������: MY_SAFE_CALL_WIN_API(SetupComm,(hComm_, txdBuffSize_, rxdBuffSize_) );
	*/
    #define MY_SAFE_CALL_WIN_API(foo,params)\
		if(!::##foo##params ) throw MyExcptionFactory::Create<MyWindowsException>\
		(::GetLastError(),GET_FLINE_,"::"#foo);

    

    /**
	* ������ ��� ��������� �������� ������� FileLine.
	*/
    namespace Private
    {
    	FileLine ExtractFileLine( unsigned line, const char* fileName );
    };
	#define GET_FLINE_ Private::ExtractFileLine(__LINE__,__FILE__)

    /**
	*   ���������� ��� ������ RethrowMyException(GET_FLINE_)
	*/
    #define MY_RETHROW_ RethrowMyException(GET_FLINE_)

    /**
	*   ���������� ��� ������ throw MyExcptionFactory::Create<MyException>(GET_FLINE_, msg )
	*/
    #define MY_THROW_(msg)\
    	throw MyExcptionFactory::Create<MyException>(GET_FLINE_, msg )

    /**
	*   ���������� ��� ������
    *   throw MyExcptionFactory::Create<�����, ����������� �� MyException>(GET_FLINE_, msg )
	*/
    #define MY_THROW_CLASS_(cls, msg)\
    	throw MyExcptionFactory::Create<##cls##>(GET_FLINE_, msg )
    

#endif
