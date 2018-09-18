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
    * Структура для инкапсуляции информации о месте возникновения исключения.
	*/
    typedef std::pair<unsigned, AnsiString> FileLine;


	/**
	* Общий базовый класс для всех исключений проекта. Все исключения должны
	* возбуждаться в виде sh_ptr<MyException>, который содержит указатель на
	* собственно исключение, производное от MyException. В принципе, отдельный
	* производный класс исключений стоит создавать только в том случае, когда у
	* пользователя должна быть возможность отличить его от "просто MyException", а
	* в остальный случаях вполне подойдет и сам MyException, означающий: "обнаружена
	* общего вида ошибка со следующим описанием".
	*/
    class MyException;

    /**
	* определение типа умного указателя на исключение
	*/
	typedef boost::shared_ptr<const MyException> ConstPMyExcpt;
    typedef boost::shared_ptr<MyException> PMyExcpt;

	/**
	* Данная функция перевозбуждает текущее исключение и возвращает созданный по
	* нему PMyExcpt. Она может быть вызвана только из блока catch. Для
	* передачи объекта location удобно использовать макрос _FLINE_.
	* Если используемая платформа предоставляет дополнительные (нестандартные)
	* возможности для получения более подробной информации о перехваченных
	* исключениях неизвестного типа, то имеет смысл написать специальную реализацию
	* данной функции, использующую эти возможности.
    */
	PMyExcpt RethrowMyException(const FileLine& location);

    /**
    *   Фабричный производитель MyException
	*   Предназначена для создания объектов вместо конструктора. Создает объект
	*   в свободной памяти по переданным месту исключения, сообщению и,
	*   возможно, вложенному исключению. Для передачи объекта loc удобно
	*   использовать макрос _FLINE_.
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
    *       Декларация MyException
    */
    class MyException : public boost::noncopyable
    {
    friend struct MyExcptionFactory;

    public:

        /** Виртуальный деструктор. */
	    virtual ~MyException(){}

	    /**
        *   Возвращает имя класса-исключения. Должна обязательно переопределяться в
	    *   производных классах.
        */
	    virtual AnsiString ClassName() const;

        /**
        *       место возникновения исключения
        */
	    const FileLine& Location() const ;
        
        /**
	    /*      сообщение
        */
	    const AnsiString& Message() const;

	    /**
        *       указатель на вложенное исключение, причину текущего, или 0
        */
        const MyException* Nested() const;

        /**
        *       указатель на старшее из вложенных исключений или this
        */
        const MyException* Root() const;

        /**
        *   Возвращает "полное описание" исключения, включающее имя класса и место
	    *   возникновения.
        */
	    AnsiString ToString() const;

	    /**
        *   Возвращает "полное описание" текущего исключения и всех исключений,
	    *   вложенных в него. Фактически, результаты применения ToString() к
	    *   каждому из исключений объединяются в одну строку.
        */
        AnsiString ToStringAll() const;
        

        /**
        *       Преобразование и проверка соответсвия типа
        */
        template<class E>
        const E* const As() const
        { return dynamic_cast<const E* const>(this); }

        template<class E>
        bool TypeOf() const
        { return dynamic_cast<const E* const>(this); }



    protected:
        /**
        *   Защищенный конструктор. Для создания объектов необходимо использовать
	    *   функцию NewMyException().
        */
        MyException(const FileLine& loc, const AnsiString& msg, const PMyExcpt nest );
    private:
        // место возникновения исключения
	    const FileLine location_;
	    // сообщение
	    const AnsiString message_;
	    // указатель на вложенное исключение, причину текущего, или 0
	    const PMyExcpt nested_;
    };

    
    /**
	*		Макросс для определения классов My##THECLASS##Exception,
    *		производных от My##THEBASECLASS
	*		с единственным защищённыи конструктором
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
	*		Общий базовый класс для инкапсуляции исключений, являющихся внешними по
	*		отношению к иерархии, порождаемой MyException.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(External, Exception );
	/**
	*      Исключение для инкапсуляции std::exception и классов, производных от него.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Std, ExternalException );
    /**
	*  Исключение для инкапсуляции Exception из VCL и классов, производных от него.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(VCL, ExternalException );
    /**
	*  Исключение для инкапсуляции Exception из VCL и классов, производных от него.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(AccessViolation, ExternalException );
    /**
	*  	Исключение для информирования о внешнем исключении, тип которого не удалось
    *	определить.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Unknown, ExternalException );

    /**
	*  	Исключение для информирования об ошибке вызова функции Win32 API
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
	*		Макрос для вызова функции Win32 API.
    *		Если результатом вызова будет FALSE, возбуждается исключение MyWindowsException.
    *		Пример: MY_SAFE_CALL_WIN_API(SetupComm,(hComm_, txdBuffSize_, rxdBuffSize_) );
	*/
    #define MY_SAFE_CALL_WIN_API(foo,params)\
		if(!::##foo##params ) throw MyExcptionFactory::Create<MyWindowsException>\
		(::GetLastError(),GET_FLINE_,"::"#foo);

    

    /**
	* Макрос для упрощения создания объекта FileLine.
	*/
    namespace Private
    {
    	FileLine ExtractFileLine( unsigned line, const char* fileName );
    };
	#define GET_FLINE_ Private::ExtractFileLine(__LINE__,__FILE__)

    /**
	*   Сокращение для вызова RethrowMyException(GET_FLINE_)
	*/
    #define MY_RETHROW_ RethrowMyException(GET_FLINE_)

    /**
	*   Сокращение для вызова throw MyExcptionFactory::Create<MyException>(GET_FLINE_, msg )
	*/
    #define MY_THROW_(msg)\
    	throw MyExcptionFactory::Create<MyException>(GET_FLINE_, msg )

    /**
	*   Сокращение для вызова
    *   throw MyExcptionFactory::Create<класс, производный от MyException>(GET_FLINE_, msg )
	*/
    #define MY_THROW_CLASS_(cls, msg)\
    	throw MyExcptionFactory::Create<##cls##>(GET_FLINE_, msg )
    

#endif
