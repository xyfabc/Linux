#include "ExportData.h"

ExportData::ExportData()
{   
}
void ExportData::writeData()
{
    //qDebug()<<"exportFileSlot"<<m_fileName;
    m_excel = new QExcel(m_fileName);
      //  qDebug()<<"init done";
    m_excel->selectSheet("Sheet1");
      //  qDebug()<<"init done";
    const int i = 0;
    m_excel->setCellString(1,i+1,"序号");
    m_excel->setCellString(1,i+2,"基波模值");
    m_excel->setCellString(1,i+3,"二次谐波模值");
    m_excel->setCellString(1,i+4,"三次谐波模值");
    m_excel->setCellString(1,i+5,"四次谐波模值");
    m_excel->setCellString(1,i+6,"五次谐波模值");
    m_excel->setCellString(1,i+7,"六次谐波模值");
    m_excel->setCellString(1,i+8,"七次谐波模值");
//    m_excel->setCellString(1,i+9,"基波模值差");
//    m_excel->setCellString(1,i+10,"二次谐波模值差");
//    m_excel->setCellString(1,i+11,"三次谐波模值差");
//    m_excel->setCellString(1,i+12,"四次谐波模值差");
//    m_excel->setCellString(1,i+13,"五次谐波模值差");
//    m_excel->setCellString(1,i+14,"六次谐波模值差");
//    m_excel->setCellString(1,i+15,"七次谐波模值差");

//    m_excel->setCellString(1,i+16,"基波/三次谐波模值");
//    m_excel->setCellString(1,i+17,"基波/五次谐波模值");
    //    qDebug()<<"init done";
    m_excel->save();
   // qDebug()<<"init done";

    DataMgr *pDataMgr = DataMgr::Inst();
    int PixIdx;
    int n = 0, k;
    if(0 == pDataMgr->m_VolCurF.cur.size()){
        return;
    }
   // TCplxFp buf[cMaxHarmnicNum] = {0};
    int length = (pDataMgr->m_size/pDataMgr->m_CZTPara.pixSmcnt);
    for(PixIdx=0; PixIdx<length; PixIdx++){
        for(k=0; k<cMaxHarmnicNum; k++){
            //TCplxFp m = pDataMgr->m_VolCurF.cur[k][PixIdx]->A/3;
            QString tmp= QString("%1").arg((int)pDataMgr->m_VolCurF.cur[k][PixIdx]->A/3);
            m_excel->setCellString(PixIdx+2,k+2,tmp);
//            tmp= QString("%1").arg((int)(m-buf[k]));
//            m_excel->setCellString(PixIdx+2,k+9,tmp);
//            if(2==k){
//                tmp= QString("%1").arg((int)(buf[0]-m));
//                m_excel->setCellString(PixIdx+2,16,tmp);
//            }else if(4 == k){
//                tmp= QString("%1").arg((int)(buf[0]-m));
//                m_excel->setCellString(PixIdx+2,17,tmp);
//            }
//            qDebug()<<"writeData!!"<<n<<m<<m-buf[k];
//            buf[k] = m;
            n++;
        }
        emit updateMsg((float)PixIdx/(float)(length-1)*100,QString("正在导出数据 ..."));
        QString tmp= QString("%1").arg(PixIdx);
        m_excel->setCellString(PixIdx+2,1,tmp);
    }
    m_excel->save();


    m_excel->close();
    qDebug()<<"write done!!";
}

ExportData::~ExportData()
{

}

void ExportData::run()
{
 writeData();
}
