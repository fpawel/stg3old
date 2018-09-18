#ifndef MY_GLOABAL_VARIABLES_OF_MODULE_DECLARATION_MACROS_HEADER_INCLUDED_____
#define MY_GLOABAL_VARIABLES_OF_MODULE_DECLARATION_MACROS_HEADER_INCLUDED_____
#include <cassert>

// �������� ������������ ������ �������
#define ASSERT_FUNCTION_CALLED_ONCE__ \
static bool wasCalledOnce=false;\
assert(!wasCalledOnce);\
wasCalledOnce = true;

// OpenModule() - �������������
// �loseModule() - ���������������
// ��� ������ ����������� ��������, ������������ � cpp �����

// � ����� Library.h
#define DECLARE_MY_MODULE_INSTANCE_(modulName)\
namespace Private##modulName##Library\
{\
	class Module##modulName##Library\
    {\
    public:\
		Module##modulName##Library()\
        {\
			if (count_++ == 0)\
            {\
				ASSERT_FUNCTION_CALLED_ONCE__;\
                OpenModule();\
            }\
		}\
		~Module##modulName##Library()\
        {\
			if (--count_ == 0)\
            {\
            	ASSERT_FUNCTION_CALLED_ONCE__;\
				CloseModule();\
            }\
		}\
    private:\
		static int count_;\
		static void OpenModule();\
		static void CloseModule();\
	};\
	static Module##modulName##Library instance##modulName##Library_;\
};

// � ����� Library.�pp

// �������� ����� ����������� ���������� � ������� ����������
#define DEFINE_MY_MODULE_INSTANCE_(modulName, InitializeFoo, FinalizeFoo )\
namespace Private##modulName##Library\
{\
	int Module##modulName##Library::count_ = 0;\
    void Module##modulName##Library::OpenModule()\
	{\
    	InitializeFoo##();\
	}\
	void Module##modulName##Library::CloseModule()\
	{\
    	FinalizeFoo##();\
	}\
};

////////////////////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// .h
//------------------------------------------------------------------------------
// ��� ������������� � ����������� ������
// DECLARE_MY_MODULE_INSTANCE_(Libraryname);

//------------------------------------------------------------------------------
// 	.cpp
//------------------------------------------------------------------------------
//	����� ����������� ���������� � ������� ����������
// 	namespace NamespaceName
// 	{
//		void Initialize();
//    	void Finalize();
//	};
// ��� ������������� � ����������� ������
// DEFINE_MY_MODULE_INSTANCE_(Libraryname, NamespaceName::Initialize, NamespaceName::Finalize);






#endif