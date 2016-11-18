//---------------------------------------------------------------------------


#pragma hdrstop

#include "math.h"
#include "uTVLogic1.h"
//#include "uRGB.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define cCtzFIntvMin 0.000001

#define cMainImgHeight cCtzFK
#define cMiBase(y) ((cMainImgHeight*3/4)-(y))
#define cMiFftB(y) (cMainImgHeight-(y)-1)

#define cBgColor TColor(0x000000)
#define cRColor TColor(0x6666CC)
#define cIColor TColor(0xCC6666)
#define cAColor TColor(0x66CC66)
#define cPColor TColor(0x006666)
#define cLineColor TColor(0x666666)

#define cPHeight 128

#define cRIBaseY(y) (64-(y))
#define cABaseY(y) (128-(y))
#define cPBaseY1(y) ((cPHeight/2)-(y))
#define cPBaseY2(y) (cPHeight-(y))

#define cRIBaseX(x) (64+(x))
#define cABaseX(x) (x)
#define cPBaseX1(x) ((cPHeight/2)+(x))
#define cPBaseX2(x) ((x))

#define cABaseMain(y) (cMainImgHeight - (y))

#define cCBaseX(x) ((cMainImgHeight/2) + (x))
#define cCBaseY(y) ((cMainImgHeight/2) - (y))



void CalcTNtoF(const TComplex *pTBuff, TCplxFp FVal, TComplex *pDstF, int N)
{
	int n;
	TCplxFp Phase;

	pDstF->R = 0;
	pDstF->I = 0;
	for(n=0; n<N; n++){
		Phase = FVal*n*2*PI/N;
		pDstF->R += pTBuff[n].R * cos(Phase);
		pDstF->I += pTBuff[n].R * sin(Phase);
	}
	int i = n;
	pDstF->R /= sqrt(N);
	pDstF->I /= sqrt(N);
	CalcCplxAP(pDstF);
}

void CalcSCZT(const TComplex *pTBuff, TComplex *pFBuff, TCplxFp F1, TCplxFp F2,
	 int N)
{
	int k;
	TCplxFp FVal;

	if(F1>F2){
		FVal = F1;
		F1 = F2;
		F2 = FVal;
	}else if(F1==F2){
		F2 += cCtzFIntvMin;
	}

	for(k=0; k<cCtzFK; k++){
		FVal = F1 + (F2-F1)*k/cCtzFK;
		CalcTNtoF(pTBuff, FVal, &pFBuff[k], N);
    }
}



TTVLogic1::TTVLogic1()
{

}

void TTVLogic1::RefParam(void)
{
    FrameSmpCnt = cPixPerFrame * PixSmpCnt;
	FrameLoadSize = FrameSmpCnt + SectSmpCnt;
	BaseF = SmpRate / SectSmpCnt;
	StepF = (CztF2 - CztF1)/cCtzFK;
}


TCplxFp TTVLogic1::GetFVal(int Fk)
{
	TCplxFp FVal;

	FVal = CztF1 + Fk*StepF;

	return(FVal);
}


void TTVLogic1::CalcCZT(void)
{
	int PixIdx;
	int n, k;
	int Color;
	static TComplex aT[cMaxSectSmpCnt];
	int IdleVal;
	int ProcVal;
	bool IsRunning;

    IdleVal = 0;
	ProcVal = 0;
	IsRunning = 1;

	for(PixIdx=0; PixIdx<cPixPerFrame; PixIdx++){
		for(n=0; n<SectSmpCnt; n++){
			aT[n].R = aSmpFrame[PixIdx*PixSmpCnt + n];
			aT[n].I = 0;
		}
		CalcSCZT(&aT[0], &aaFDat[PixIdx][0], CztF1/BaseF,
			CztF2/BaseF, SectSmpCnt);
		//
//		ProcVal += SectSmpCnt;
//		if(ProcVal >= IdleVal){
//			IsRunning = OnCalcProgress((PixIdx*100)/cPixPerFrame);
//			IdleVal += 4096;
//		}
//		if(!IsRunning){
//			break;
//		}
	}
}


//void TTVLogic1::DrawMainCZT(TCanvas *Canvas)
//{
//	int PixIdx;
//	int k;
//	int Color;
//	int SectSmpCnt;

//    Canvas->Brush->Color = cBgColor;
//	Canvas->FillRect(Canvas->ClipRect);

//	for(PixIdx=0; PixIdx<cPixPerFrame; PixIdx++){
//		for(k=0; k<cCtzFK; k++){
//			Color = GetCplxColor(aaFDat[PixIdx][k], BlendVal, ALimitVal);
//			Canvas->Pixels[PixIdx][cMiFftB(k)] = TColor(Color);
//		}
//	}
//}

//void TTVLogic1::DrawMainTA(TCanvas *Canvas)
//{
//	int PixIdx;
//	int n, k;
//	int Color;
//	int SmpMin;
//	int SmpMax;
//	int SmpVal;

//	Canvas->Brush->Color = cBgColor;
//	Canvas->FillRect(Canvas->ClipRect);

//	Canvas->Pen->Color = cAColor;
//	Canvas->MoveTo(-1, cABaseMain(0));
//	for(PixIdx=0; PixIdx<cPixPerFrame; PixIdx++){
//		SmpMin = cMainImgHeight;
//		SmpMax = 0;
//		for(n=0; n<PixSmpCnt; n++){
//			SmpVal = aSmpFrame[PixIdx*PixSmpCnt + n];
//			if(SmpVal<SmpMin){
//				SmpMin = SmpVal;
//			}
//			if(SmpVal>SmpMax){
//				SmpMax = SmpVal;
//			}
//		}
//		Canvas->LineTo(PixIdx, cABaseMain(SmpMin));
//		Canvas->LineTo(PixIdx, cABaseMain(SmpMax));
//	}
//}

//void TTVLogic1::DrawTPSig(TCanvas *Canvas, int Fk)
//{
//    int i;
//    int Y;
//	TCplxFp PhaseY;
//	int CplxColor;

//    Canvas->Brush->Color = cBgColor;
//    Canvas->FillRect(Canvas->ClipRect);

//	Canvas->Pen->Color = cLineColor;
//	Y = ALimitVal/AScale;
//	Canvas->MoveTo(0, cABaseY(Y));
//	Canvas->LineTo(cPixPerFrame, cABaseY(Y));

//	Canvas->Pen->Color = cAColor;
//	Canvas->MoveTo(-1, cABaseY(0));
//	for(i=0; i<cPixPerFrame; i++){
//		Y = cABaseY(aaFDat[i][Fk].A/AScale);
//		//Canvas->Pixels[i][Y] = cAColor;
//		Canvas->LineTo(i, Y);
//	}
//    Canvas->Pen->Color = cPColor;
//	for(i=0; i<cPixPerFrame; i++){
//		PhaseY = (aaFDat[i][Fk].P*cPHeight/4)/PI;
//		Canvas->Pixels[i][cPBaseY1(PhaseY)] = cPColor;
//        Canvas->Pixels[i][cPBaseY2(PhaseY)] = cPColor;
//	}
//}
//void TTVLogic1::ExportTPSig(TFileStream *Log,int Fk)
//{
//	int i;
//	UnicodeString AMsg;
//	AMsg = "频率";
//	AMsg +="		";
//	AMsg +="时间量";
//	AMsg +="		";
//	AMsg +="幅值";
//	AMsg +="		";
//	AMsg +="相位";
//	AMsg += "\r\n";
//	Log->Write(AMsg.t_str(), AMsg.Length());
//	for(i=0; i<cPixPerFrame; i++){
//		int Y = cABaseY(aaFDat[i][Fk].A);
//		TCplxFp PhaseY = (aaFDat[i][Fk].P*cPHeight/4)/PI  ;
//		AMsg = TVLogic1->GetFVal(Fk);
//		AMsg +="		";
//		AMsg +=i;
//		AMsg +="		";
//		AMsg +=Y;
//		AMsg +="		";
//		AMsg +=PhaseY;
//		AMsg += "\r\n";

//		Log->Write(AMsg.t_str(), AMsg.Length());
//	}
//}

//void TTVLogic1::DrawFPSig(TCanvas *Canvas, int Tn)
//{
//    int i;
//	int X;
//	TCplxFp PhaseX;
//	int CplxColor;

//    Canvas->Brush->Color = cBgColor;
//    Canvas->FillRect(Canvas->ClipRect);

//	Canvas->Pen->Color = cLineColor;
//	X = ALimitVal/AScale;
//	Canvas->MoveTo(cABaseX(X), 0);
//	Canvas->LineTo(cABaseX(X), cCtzFK);

//	Canvas->Pen->Color = cAColor;
//	Canvas->MoveTo(cABaseX(0), -1);
//	for(i=0; i<cCtzFK; i++){
//		X = cABaseX(aaFDat[Tn][cCtzFK-i-1].A/AScale);
//		//Canvas->Pixels[i][Y] = cAColor;
//		Canvas->LineTo(X, i);
//	}
//	Canvas->Pen->Color = cPColor;
//	for(i=0; i<cCtzFK; i++){
//		PhaseX = (aaFDat[Tn][cCtzFK-i-1].P*cPHeight/4)/PI;
//		Canvas->Pixels[cPBaseX1(PhaseX)][i] = cPColor;
//		Canvas->Pixels[cPBaseX2(PhaseX)][i] = cPColor;
//	}
//}

//void TTVLogic1::DrawCstl(TCanvas *Canvas, int Fk)
//{
//	int i;
//	TCplxFp X,Y;
//	TCplxFp ALimitR;
//	//TCplxFp A,P;
//	int Color;


//    Canvas->Brush->Color = cBgColor;
//	Canvas->FillRect(Canvas->ClipRect);
//	Canvas->Pen->Color = cLineColor;

//	ALimitR = ALimitVal/AScale;
//	Canvas->Ellipse(cCBaseX(0)-ALimitR, cCBaseY(0)-ALimitR,
//		cCBaseX(0)+ALimitR, cCBaseY(0)+ALimitR);

//	for(i=0; i<cCtzFK; i++){
//		X = cCBaseX(aaFDat[i][Fk].R / AScale);
//		Y = cCBaseY(aaFDat[i][Fk].I / AScale);
//		//A = aaFDat[i][Fk].A / AScale;
//		//P = aaFDat[i][Fk].P;
//		//X = cCBaseX(A*cos(P));
//		//Y = cCBaseY(A*sin(P));
//		Color = 0x010000*(cCtzFK-i-1);
//		Color += 0x000101*i;
//		Canvas->Pixels[X][Y] = TColor(Color);
//	}

//}




