//---------------------------------------------------------------------------
#ifndef uChartHelperH
#define uChartHelperH
//---------------------------------------------------------------------------
#include <Chart.hpp>
//---------------------------------------------------------------------------
//std
#include <utility>
#include <vector>

// boost
#include "boost\noncopyable.hpp"
//#include "boost\shared_ptr.hpp"

#include "free_types_.h"

bool IsPointInChartRect( TChart* chrt, int x, int y );

struct ChartAxisOrders
{
	double minX, minY, maxX, maxY, lenX, lenY;
};

ChartAxisOrders GetChartAxisOrders(TChart *chrt);

std::pair<double,double> GetYLimits(TChart *chrt);
std::pair<double,double> GetYLimitsOfSeries(TChartSeries* ser);

void SetAxisMin( TChartAxis* axis, double val );
void SetAxisMax( TChartAxis* axis, double val );
void SetAxisOrders( TChartAxis* axis, double minV, double maxV );

TChartSeries* GetFirstActiveChartSeries(TChart *chrt);

// получить индекс y по x или -1
int GetYValueChartSeries(TChartSeries* ser, double x);

void DrawSeriesValuesPoints(TChartSeries* ser, TColor clr, unsigned radius);

enum AxOrdr_ { ax_Max = true, ax_Min = false } ;
void IncAxOrder( TChartAxis *ax, AxOrdr_ ordr, bool inc );

///////////////////////////////////////////////////////////////////////////////
// ѕомошник дл€ отображени€ курсора графика
///////////////////////////////////////////////////////////////////////////////
void SetDrawingCrossHairPen(TChart *chrt, int width = 1);
void DrawCrossHair(TChart *chrt, int ax, int ay, TColor horClr, TColor verClr);

void DrawSeriesXMarks(TChartSeries* ser, TChartAxis* ax, const VDouble& marks,
	const VDouble& x0, const VDouble& x1, const std::vector<AnsiString>& strs );


class CrossHairHelper : public boost::noncopyable
{
public:

	explicit CrossHairHelper( TChart* chrt, TColor horClr, TColor verClr );

    void OnMouseMove(int x, int y);

    void Hide()
    {
		if(x_==-1) return;
		DrawCross( x_, y_ );
    	x_=-1;
	}

    void Reset() { x_ = -1; }

    void (__closure *OnMouseOcurence)(bool);

private:
	TChart* chrt_;
    TCanvas3D* cnv_;
    // позици€
    int x_, y_;

    const TColor horClr_, verClr_;

	void OnChartMouseOcurence( TWinControl* chrt, bool isEntered)
    {
    	if( !isEntered ) Hide();
        if( OnMouseOcurence ) OnMouseOcurence(isEntered);
    }
    
    void DrawCross(int ax, int ay)
    {
    	SetDrawingCrossHairPen(chrt_);
        DrawCrossHair(chrt_, ax, ay, horClr_, verClr_);
    }
};


//---------------------------------------------------------------------------
#endif
