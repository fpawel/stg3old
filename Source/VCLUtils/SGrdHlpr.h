//---------------------------------------------------------------------------
#ifndef SGrdHlprH
#define SGrdHlprH
//---------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"

//---------------------------------------------------------------------------
#include <Grids.hpp>

//my
#include "..\my_include\MyDynamicBitset.h"
//---------------------------------------------------------------------------
//#include "boost/tuple/tuple.hpp"
//#include "boost\tuple\tuple_comparison.hpp"
//---------------------------------------------------------------------------
namespace my
{
	namespace GridLineT
	{
		enum{ top, left };
	};
	//using boost::tuples::tuple;
	//using boost::tuples::make_tuple;
	//using boost::tuples::tie;
	//---------------------------------------------------------------------------
	//       ��������� ����� ��� ������ � �������� ������ TStringGrid
	//---------------------------------------------------------------------------
	//  ������ �������
    namespace Grd
    {
    	struct Cell
    	{
    		TStringGrid* grd_;
        	int col_, row_;
        	explicit Cell( TStringGrid* grd = NULL, int col = 0, int row = 0 );
    	};
        void SetTxt(Cell& cell, const AnsiString& s);
        AnsiString GetTxt(const Cell& cell);

        // ���������� ������ col,row � ������� g
		void RedrawCell(Cell cell);
        void RedrawCell(TStringGrid* grd, int col, int row);

        void RedrawRow(TStringGrid* grd, int row);
        void RedrawCol(TStringGrid* grd, int row);


        // �������� �� ������ col,row � ������������� ������� �������
		bool IsCellFixed(const Cell& cell);
        bool IsCellInEditMode( const Cell& cell );
        void SetCellIfNotEdited(Cell cell, AnsiString val);

        // ���������� ������������� left, top, right, bottom � ������� g
		void RedrawRect(TStringGrid* g, TRect rect);

        // ���������� ������� ������������� ��������������� ����� �������
        TRect VisRect( TStringGrid* g);

        // ������ �� ��������
        bool IsRangeSelected( const TStringGrid* g );


	//---------------------------------------------------------------------------

	// ���������� ������� ������������� ��������������� ����� �������
	TRect VisRect(TStringGrid* g);

	// ������� ������� ������������� ��� ���
	void SetEditing(TStringGrid* grd, bool fl);

	// ��������� �� ������
	bool FilledRow( TStringGrid* grd, int row);

	// ������ ��� ��������
	TPoint HotCell(TStringGrid* grd);

	void ClearTStringGrid( TStringGrid* grd, int cl0 = 0, int ro0 = 0 );

    // �������� ��������������� ������� �����
    AnsiString GetFormatedVisibleText( const TStringGrid* grd,
    	unsigned col_from, unsigned col_to,
        unsigned row_from, unsigned row_to);

    void SetColVis(const TStringGrid* grd, unsigned col, bool vis);
    void SetRowVis(const TStringGrid* grd, unsigned row, bool vis);

    int GetLastVisCol(TStringGrid* grd)
    {
    	int cl=grd->ColCount-1;
    	for( ; cl>-1; --cl ) if( grd->ColWidths[cl]>0 ) break;
        return cl;
    }

    int GetLastVisRow(TStringGrid* grd)
    {
    	int ro=grd->RowCount-1;
    	for( ; ro>-1; --ro ) if( grd->RowHeights[ro]>0 ) break;
        return ro;
    }

    AnsiString GetVisColsInfo(TStringGrid* grd);
    void SetVisColsInfo(TStringGrid* grd, const AnsiString&);

    // ������� ������ ��� ��������� ������� ��������
    void SetupVisibleCols(TStringGrid *grd);
    void SetupVisibleColsOnMouseDownRow0(TObject *grd, TMouseButton Button, TShiftState Shift, int X, int Y);
    void SetupVisibleRowsOnMouseDownCol0(TObject *grd, TMouseButton Button, TShiftState Shift, int X, int Y);

    AnsiString GetVisRowsInfo(TStringGrid* grd);
    void SetVisRowsInfo(TStringGrid* grd, const AnsiString&);

    void ClearSelection(const TStringGrid* grd);

    // ���������� � �����
    void CopyCellsToClipboard( const TStringGrid* grd );
    void PastCellsFromClipboard(const TStringGrid* grd);

    template<typename T>
    void SetColumn( TStringGrid* grd, const int cl, T str, T strEnd, int row0 = 0)
    {
    	for( int ro=row0; ro<grd->RowCount && str!=strEnd ; ++ro )
        	grd->Cells[cl][ro] = *str++;
    }


    template<typename T>
    void SetRow( TStringGrid* grd, const int ro, T str, T strEnd, int col0 = 0)
    {
    	for( int cl=col0; cl<grd->ColCount && str!=strEnd ; ++cl )
        	grd->Cells[cl][ro] = *(str++);
    }

    void TieWithColsVisiblePopup( TStringGrid* grd, TNotifyEvent onPopup = NULL );
    void TieWithRowsVisiblePopup( TStringGrid* grd, TNotifyEvent onPopup = NULL  );

    void DeleteRow(TStringGrid* grd, int row, int count);
    void DeleteCol(TStringGrid* grd, int col, int count);

    void SetCellByTtl(TStringGrid *grd, const AnsiString& ttl, int row, const AnsiString& s);
	AnsiString GetCellByTtl(TStringGrid *grd, const AnsiString& ttl, int row);

    TPopupMenu* GetPopupMenu(TStringGrid *grd);

    void AddCopyItemToPopupMenu(TStringGrid *grd);

    unsigned GetRowsHeight(TStringGrid *grd);



    }; // namespace Grd
}; // namespace my



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
