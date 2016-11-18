#include "DataMgr.h"

void DataMgr::CalcCplxAP(TComplex *pCplx)
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
void DataMgr::CalcTNtoF(QList<TComplex*> *pTBuff, int k, TComplex *pDstF, int PixIdx,int N)
{
    int n;
    TCplxFp Phase;
    TCplxFp  FVal = (k+1)*50*(TCplxFp)m_CZTPara.sectSmpCnt*(TCplxFp)m_CZTPara.aLimit/(TCplxFp)m_CZTPara.smRate;
   // qDebug()<<"FVal"<<50*(TCplxFp)m_CZTPara.sectSmpCnt*(TCplxFp)m_CZTPara.aLimit/(TCplxFp)m_CZTPara.smRate;

    pDstF->R = 0;
    pDstF->I = 0;
    for(n=0; n<N; n++){
        Phase = FVal*n*2*PI/N;
        if((PixIdx*m_CZTPara.pixSmcnt+n)<pTBuff->size()){
            pDstF->R += pTBuff->at(PixIdx*m_CZTPara.pixSmcnt+n)->R * cos(Phase);
            pDstF->I += pTBuff->at(PixIdx*m_CZTPara.pixSmcnt+n)->R * sin(Phase);
        }
    }
    pDstF->R /= sqrt(N);
    pDstF->I /= sqrt(N);

    CalcCplxAP(pDstF);
    //qDebug()<<"频率"<<FVal<<"实部"<< pDstF->R<<"虚部"<<pDstF->I<<"幅值" <<pDstF->A<<"相位"<<pDstF->P;
}

DataMgr::DataMgr()
{    
    //m_CZTPara.init();
    m_CZTPara.aLimit = 1;
    m_CZTPara.pixSmcnt = 82;
    m_CZTPara.sectSmpCnt = 409;
    //m_CZTPara.ascale = 2;
    m_CZTPara.smRate = 2048;
    IsLoadfile = false;
    IsCalc = false;
    m_offset = 0;
    initView();
    IsCalc = true;
}

DataMgr *DataMgr::Inst()
{
    static DataMgr *pDataMgr = NULL;
    if(!pDataMgr){
     pDataMgr = new DataMgr();
    }
    return  pDataMgr;
}
void DataMgr::CalcCZT(int pos)
{
   // qDebug()<<"CalcCplxAP_befor";
    int k = 0;
    int PixIdx = 0;
    if(m_calcCTZstatus.value(pos)){
        return;
    }
    isPosChange = false;
    int offset = pos*cPixPerFrame;
    for(PixIdx=offset; PixIdx<offset+cPixPerFrame; PixIdx++){
        for(k=0; k<cMaxHarmnicNum; k++){
            if(PixIdx<m_size-m_CZTPara.sectSmpCnt){
                if(isPosChange){
                    qDebug()<<"isPosChange";
                    return;
                }
                CalcTNtoF(&m_VolCurT.curBA, k, m_VolCurF.cur[k][PixIdx],PixIdx,m_CZTPara.sectSmpCnt);
                CalcTNtoF(&m_VolCurT.volBA, k, m_VolCurF.vol[k][PixIdx],PixIdx,m_CZTPara.sectSmpCnt);
                m_fpp[k][PixIdx]->P = m_VolCurF.cur[k][PixIdx]->P - m_VolCurF.vol[k][PixIdx]->P;
                TCplxFp c =  m_VolCurF.cur[k][PixIdx]->R;
                TCplxFp d =  m_VolCurF.cur[k][PixIdx]->I;
                TCplxFp a =  m_VolCurF.vol[k][PixIdx]->R;
                TCplxFp b =  m_VolCurF.vol[k][PixIdx]->I;
                qDebug()<<"CalcCplxAP_befor_befor"<<k<<m_fpp[k][PixIdx]->P;
                m_VolCurF.imped[k][PixIdx]->R = (a*c+b*d)/(c*c+d*d);
                m_VolCurF.imped[k][PixIdx]->I = (b*c-a*d)/(c*c+d*d);
               // qDebug()<<"CalcCplxAP_befor"<<m_VolCurF.imped[k][PixIdx]->R<<m_VolCurF.imped[k][PixIdx]->I<<m_VolCurF.imped[k][PixIdx]->A<<m_VolCurF.imped[k][PixIdx]->P;
                CalcCplxAP(m_VolCurF.imped[k][PixIdx]);
                //qDebug()<<"CalcCplxAP"<<m_VolCurF.imped[k][PixIdx]->A<<m_VolCurF.imped[k][PixIdx]->P;

            }
            if(0 == PixIdx%100){
             emit calcCZTpartOver();
            }

        }
    }
     m_calcCTZstatus.insert(pos,true);
    qDebug()<<"Over"<<pos;
}
void DataMgr::RefParam(void)
{
//    m_CZTPara.FrameSmpCnt = cPixPerFrame * m_CZTPara.pixSmcnt;
//    m_CZTPara.FrameLoadSize = m_CZTPara.FrameSmpCnt + m_CZTPara.sectSmpCnt;
//    m_CZTPara.BaseF = m_CZTPara.smRate / m_CZTPara.sectSmpCnt;  //实际的
//    m_CZTPara.StepF = (m_CZTPara.CztF2 - m_CZTPara.CztF1)/cCtzFK;
}

void DataMgr::CalcRe()
{
    if(IsLoadfile){
        TComplex *val;
       m_VolCurT.ReBA.clear();
       for(int i=0;i<m_VolCurT.volBA.size();i++){
          val = new TComplex;
          m_VolCurT.ReBA.insert(i,val);
          m_VolCurT.ReBA[i]->R =  m_VolCurT.volBA[i]->R/m_VolCurT.curBA[i]->R;
          m_VolCurT.ReBA[i]->I = 0;
          qDebug()<<"CalcRe"<<m_VolCurT.volBA[i]->R<<m_VolCurT.curBA[i]->R<<val->R;        
        }
       // QMessageBox::information(0, "系统提示", "时域计算完毕！");
    }
}

void DataMgr::CalcImped()
{

}

void DataMgr::CalcTAP()
{
  for(int i=0;i<m_size;i++){
      CalcCplxAP(m_VolCurT.curBA[i]);
      CalcCplxAP(m_VolCurT.volBA[i]);
      CalcCplxAP(m_VolCurT.ReBA[i]);
  }
}

void DataMgr::CalcPID()
{
}

void DataMgr::SetSize(int size)
{
    m_size = size;
}

void DataMgr::initView()
{
    m_ViewList.clear();
    VeiwSourPara *para1 = new VeiwSourPara;
    para1->ID = T_CUR_R;
    para1->name = "时域电流实部";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_R;
    m_ViewList.insert(0,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_CUR_I;
    para1->name = "时域电流虚部";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_I;
    m_ViewList.insert(1,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_CUR_P;
    para1->name = "时域电流相位";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(2,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_CUR_A;
    para1->name = "时域电流模值";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_A;
    m_ViewList.insert(3,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_VOL_R;
    para1->name = "时域电压实部";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_R;
    m_ViewList.insert(4,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_VOL_I;
    para1->name = "时域电压虚部";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_I;
    m_ViewList.insert(5,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_VOL_P;
    para1->name = "时域电压相位";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(6,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_VOL_A;
    para1->name = "时域电压模值";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_A;
    m_ViewList.insert(7,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_RE_R;
    para1->name = "时域电阻实部";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_R;
    m_ViewList.insert(8,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_RE_I;
    para1->name = "时域电阻虚部";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_I;
    m_ViewList.insert(9,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_RE_P;
    para1->name = "时域电阻相位";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(10,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_RE_A;
    para1->name = "时域电阻模值";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_A;
    m_ViewList.insert(11,para1);

    para1 = new VeiwSourPara;
    para1->ID = F_CUR_R;
    para1->name = "频域电流实部";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_R;
    m_ViewList.insert(12,para1);

    para1 = new VeiwSourPara;
    para1->ID = F_CUR_I;
    para1->name = "频域电流虚部";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_I;
    m_ViewList.insert(13,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_CUR_P;
    para1->name = "频域电流相位";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(14,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_CUR_A;
    para1->name = "频域电流幅值";
    para1->dataType = DATA_TYPE_CUR;
    para1->ripaType = RIPA_TYPE_A;
    m_ViewList.insert(15,para1);

    para1 = new VeiwSourPara;
    para1->ID = F_VOL_R;
    para1->name = "频域电压实部";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_R;
    m_ViewList.insert(16,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_VOL_I;
    para1->name = "频域电压虚部";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_I;
    m_ViewList.insert(17,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_VOL_P;
    para1->name = "频域电压相位";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(18,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_VOL_A;
    para1->name = "频域电压模值";
    para1->dataType = DATA_TYPE_VOL;
    para1->ripaType = RIPA_TYPE_A;
    m_ViewList.insert(19,para1);

    para1 = new VeiwSourPara;
    para1->ID = F_IMPED_R;
    para1->name = "阻抗实部";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_R;
    m_ViewList.insert(20,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_IMPED_I;
    para1->name = "阻抗虚部";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_I;
    m_ViewList.insert(21,para1);

    para1 = new VeiwSourPara;
    para1->ID = F_IMPED_P;
    para1->name = "阻抗相位";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(22,para1);
    para1 = new VeiwSourPara;
    para1->ID = F_IMPED_A;
    para1->name = "阻抗模值";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_A;
    m_ViewList.insert(23,para1);

    para1 = new VeiwSourPara;
    para1->ID = T_LINE;
    para1->name = "线";
    para1->dataType = DATA_TYPE_LINE;
    para1->ripaType = RIPA_TYPE;
    m_ViewList.insert(24,para1);

    para1 = new VeiwSourPara;
    para1->ID = F_P_P;
    para1->name = "相位差";
    para1->dataType = DATA_TYPE_IMPED;
    para1->ripaType = RIPA_TYPE_P;
    m_ViewList.insert(26,para1);

}

void DataMgr::initCTZ()
{
    m_calcCTZstatus.clear();
    for(int i=0;i<m_size/cPixPerFrame;i++){
       m_calcCTZstatus.insert(i,false);
    }
    m_VolCurF.cur.clear();
    for(int j=0;j<cMaxHarmnicNum;j++){
       QList<TComplex*>  list ;
       for(int i=0;i<m_size;i++){
            TComplex *tmp = new TComplex;
            list.append(tmp);
        }
       m_VolCurF.cur.append(list);
    }
    m_VolCurF.vol.clear();
    for(int j=0;j<cMaxHarmnicNum;j++){
       QList<TComplex*>  list;
       for(int i=0;i<m_size;i++){
            TComplex *tmp = new TComplex;
            list.append(tmp);
        }
       m_VolCurF.vol.append(list);
    }
    m_VolCurF.imped.clear();
    for(int j=0;j<cMaxHarmnicNum;j++){
       QList<TComplex*> list ;
       for(int i=0;i<m_size;i++){
            TComplex *tmp = new TComplex;
            list.append(tmp);
        }
       m_VolCurF.imped.append(list);
    }
    m_fpp.clear();
    for(int j=0;j<cMaxHarmnicNum;j++){
       QList<TComplex*> list;
       for(int i=0;i<m_size;i++){
            TComplex *tmp = new TComplex;
            list.append(tmp);
        }
       m_fpp.append(list);
    }
}



VeiwTypeID DataMgr::GetViewIdByName(QString name)
{
    QMapIterator<int,VeiwSourPara*> i(m_ViewList);
    while (i.hasNext()) {
        i.next();
        if(name == i.value()->name){
            return i.value()->ID;
        }
    }
    return UnKnow;
}




