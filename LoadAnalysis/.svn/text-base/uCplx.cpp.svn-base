//---------------------------------------------------------------------------




#include "uCplx.h"
#include "math.h"

//---------------------------------------------------------------------------



void CalcCplxAP(TComplex *pCplx)
{
    pCplx->A = sqrt(pCplx->R*pCplx->R + pCplx->I*pCplx->I);
    if(0!=pCplx->R){
        pCplx->P = atan(pCplx->I/pCplx->R);
    }else{
        pCplx->P = PI/2;
    }
    if(pCplx->R<0){
            pCplx->P += PI;
    }else{
        if(pCplx->I<0){
            pCplx->P += 2*PI;
        }
    }
}

void CplxAdd(TComplex *pDst, const TComplex *pSrc1, const TComplex *pSrc2)
{
	pDst->R = pSrc1->R + pSrc2->R;
	pDst->I = pSrc1->I + pSrc2->I;
}

void CplxSub(TComplex *pDst, const TComplex *pSrc1, const TComplex *pSrc2)
{
	pDst->R = pSrc1->R - pSrc2->R;
	pDst->I = pSrc1->I - pSrc2->I;
}

//(a+ib)(c+id) = (ac-bd)+i(ad+bc)
void CplxMul(TComplex *pDst, const TComplex *pSrc1, const TComplex *pSrc2)
{
	TComplex Cplx1;

	Cplx1.R = pSrc1->R*pSrc2->R - pSrc1->I*pSrc2->I;
	Cplx1.I = pSrc1->R*pSrc2->I + pSrc1->I*pSrc2->R;
	*pDst = Cplx1;
}

