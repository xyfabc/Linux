#ifndef PUB_H
#define PUB_H

#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QColor>

#define MaxWidth 256
#define MaxHeight 256
#define FileMaxLength 5000000    //载入文件的最大字节数

#define cCtzFK 200
#define cPixPerFrame 1024
#define cMaxTScaleLevel 16
#define cMaxHarmnicNum 7
//#define PixSmpCnt 1
#define cMaxSectSmpCnt (1<<cMaxTScaleLevel)
#define cMaxFrameSmpCnt (cMaxSectSmpCnt*cPixPerFrame)
#define cCtzFIntvMin 0.000001


#define cMainImgHeight cCtzFK
#define cMiBase(y) ((cMainImgHeight*3/4)-(y))
#define cMiFftB(y) (cMainImgHeight-(y)-1)


#pragma  pack(push,4)

typedef float TCplxFp;
typedef unsigned char uint8;
#define PI 3.1415926535897932384626433832795028841971
#define CUR_K 0.01877131058
#define VOL_K 0.375538733

typedef struct{
    TCplxFp R;  //实部
    TCplxFp I;  //虚部
    TCplxFp A;  //幅值
    TCplxFp P;  //相位
}TComplex;

typedef struct{
    QList< QList<TComplex*> > cur;
    QList< QList<TComplex*> > vol;
    QList< QList<TComplex*> > imped; //阻抗
}VolCurF;

typedef struct{
    QList<TComplex*> curBA;
    QList<TComplex*> volBA;
    QList<TComplex*> ReBA;
}VolCurT;

typedef struct {
     int aLimit;
//     float freqCenter;
//     float freqRange;
     int pixSmcnt;
     int sectSmpCnt;
//     int ascale;
     int smRate;
//     TCplxFp CztF1;
//     TCplxFp CztF2;
//     TCplxFp BaseF;
//     TCplxFp StepF;
//     int FrameSmpCnt;
//     int FrameLoadSize;
}CZTPara;

typedef struct{
    int vol_k;
    int vol_c;
    int cur_k;
    int cur_c;
}ConvertPara;

typedef enum{
    UnKnow      = -1,
    T_CUR_R       = 0,
    T_CUR_I       = 1,
    T_CUR_P       = 2,
    T_CUR_A       = 3,
    T_VOL_R       = 4,
    T_VOL_I       = 5,
    T_VOL_P       = 6,
    T_VOL_A       = 7,
    T_RE_R        = 8,
    T_RE_I        = 9,
    T_RE_P        = 10,
    T_RE_A        = 11,
    F_CUR_R     = 12,
    F_CUR_I     = 13,
    F_CUR_P     = 14,
    F_CUR_A     = 15,
    F_VOL_R     = 16,
    F_VOL_I     = 17,
    F_VOL_P     = 18,
    F_VOL_A     = 19,
    F_IMPED_R   = 20,
    F_IMPED_I   = 21,
    F_IMPED_P   = 22,
    F_IMPED_A   = 23,
    T_LINE      = 24,
    T_PID       = 25,
    F_P_P       = 26,
}VeiwTypeID;

typedef enum{
    T_DOM       = 0,
    F_DOM_R     = 1,
    F_DOM_I     = 2,
    F_DOM_A     = 3,
    F_DOM_P     = 4,
    T_DOM_PID   = 5,
    T_DOM_LINE  = 6,
}VeiwDomaType;

typedef enum{
    DATA_TYPE_CUR     = 0,
    DATA_TYPE_VOL     = 1,
    DATA_TYPE_IMPED   = 2,
    DATA_TYPE_LINE    = 3,
}DataType;
typedef enum{
    HARM_TYPE_ONE     = 0,
    HARM_TYPE_TWO     = 1,
    HARM_TYPE_THREE   = 2,
    HARM_TYPE_FOUR    = 3,
    HARM_TYPE_FIVE    = 4,
    HARM_TYPE_SIX     = 5,
    HARM_TYPE_SEVEN   = 6,
    HARM_TYPE_T       = 7,
}HarmType;

typedef enum{
    RIPA_TYPE     = -1,
    RIPA_TYPE_R     = 0,
    RIPA_TYPE_I     = 1,
    RIPA_TYPE_P     = 2,
    RIPA_TYPE_A     = 3,
    RIPA_TYPE_A_A   = 4,
}RIPAType;

typedef struct{
    VeiwTypeID ID;
    VeiwDomaType Type;
    RIPAType ripaType;
    DataType dataType;
    QString name;
    void init(){
         ID = UnKnow;
         Type = T_DOM;
         name = "";
         ripaType = RIPA_TYPE_R;
         dataType = DATA_TYPE_CUR;
    }
}VeiwSourPara;

typedef struct{
    VeiwTypeID PIDSour;
    int IDType;
    int mid;
    float p1;
    float p2;
    void init(){
        PIDSour = UnKnow;
        IDType = 0;
        mid = 5;
        p1 = 1;
        p2 = 1;
    }
}PIDPara;

typedef struct{
    int ID; //
    int C;      //
    float K;      //
    int CentralAxis;
    int Size;
    int Harm;
    VeiwSourPara VeiwID;
    int PixSmcnp;
    QColor Color;
    PIDPara pidPara;
    QString viewName;
    void init(){
         ID = 0; //
         C = 100;      //
         K = 20;      //
         CentralAxis = 100;
         Size = 200;
         Harm = 1;
         VeiwID;
         PixSmcnp = 1;
         Color = Qt::yellow;
         pidPara.init();
    }
}VeiwPara;

#pragma pack(pop)
#endif // PUB_H
