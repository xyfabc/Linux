#include "CalcCZTPartTh.h"
#include "DataMgr.h"

CalcCZTPartTh::CalcCZTPartTh()
{
}

void CalcCZTPartTh::run()
{
    DataMgr *pDataMgr = DataMgr::Inst();
    pDataMgr->CalcCZT(pos);

    emit calcCZTpartOver();
}
