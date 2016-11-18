#ifndef DATAMGR_H
#define DATAMGR_H

#include <QFile>
#include "Pub.h"
#include <math.h>
#include <QObject>
#include <QMutex>
#include <QMap>

class DataMgr :public QObject
{
    Q_OBJECT
public:
    DataMgr();
    static DataMgr* Inst();
    void RefParam(void);        //刷新参数
    void CalcRe();              //计算时域电阻
    void CalcHarmnic();         //计算谐波分量
    void PID();                 //计算PID
    void CalcImped();             //计算阻抗
    void CalcTAP();             //
    void CalcPID();
    void SetSize(int);
    void initView();
    void initCTZ();
    void CalcCZT(int pos);             //计算频域
    void CalcP_P();
    void CalcCplxAP(TComplex *pCplx);
    void CalcTNtoF(QList<TComplex*> *pTBuff, int k, TComplex *pDstF, int PixIdx,int N);

public:
    VolCurT m_VolCurT;          //时域电流电压
    VolCurF m_VolCurF;          //频域电流电压
    QList< QList<TComplex*> > m_fpp;
   // QList< QList<TComplex*> > m_Imped; //阻抗
//    TComplex **m_Imped;          //阻抗
    QMap<int,QList<TCplxFp> >m_PID;
    ConvertPara m_ConvertPara;  //时域转换参数
    CZTPara m_CZTPara;          //频域转换参数
    int m_offset;               //时域偏移标志
    bool IsLoadfile;            //载入文件标志
    bool IsCalc;
    int m_size;                 //电流电压的长度
    QMutex calcMutex;
    QMap<int,VeiwSourPara*> m_ViewList;
    QMap<int,VeiwPara*> m_ViewParaList;

    VeiwTypeID GetViewIdByName(QString name);
    int m_lenth;
    QMap<int ,bool> m_calcCTZstatus;
    int m_veiwWidth;
    bool isPosChange;


   // int m_harm;



private:
    //char fileBuf[1024];

signals:
    void progressBarSignal(int,QString);
    void calcCZTpartOver();


};

#endif // DATAMGR_H
