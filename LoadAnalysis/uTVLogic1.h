//---------------------------------------------------------------------------

#ifndef uTVLogic1H
#define uTVLogic1H
//---------------------------------------------------------------------------




#include "uCplx.h"

#include "Pub.h"


class TTVLogic1
{
private:
	int FTScale;
public:
     TTVLogic1();
	uint8 aSmpFrame[cMaxFrameSmpCnt];
	TComplex aaFDat[cPixPerFrame][cCtzFK];

	int	CurTn;
	int CurFk;

	bool IsTArea;
	int BlendVal;
	int ALimitVal;
	int SmpRate;
	TCplxFp CztF1;
	TCplxFp CztF2;
	TCplxFp BaseF;
	TCplxFp StepF;
	int AScale;
	int PixSmpCnt;
	int SectSmpCnt;
	int FrameSmpCnt;
	int FrameLoadSize;
	void CalcCZT(void);
//	void DrawMainTA(TCanvas *Canvas);
//	void DrawMainCZT(TCanvas *Canvas);
//	void DrawTPSig(TCanvas *Canvas, int Fk);
//	void ExportTPSig(TFileStream *Log,int Fk);
//	void DrawFPSig(TCanvas *Canvas, int Tn);
//	void DrawCstl(TCanvas *Canvas, int Fk);
	void RefParam(void);
	TCplxFp GetFVal(int Fk);
};






#endif
