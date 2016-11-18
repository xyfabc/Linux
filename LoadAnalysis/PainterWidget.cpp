#include "PainterWidget.h"
#include "ui_PainterWidget.h"
#include <QPainter>
#include "DataMgr.h"
#include <QDebug>



PainterWidget::PainterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PainterWidget)
{
    ui->setupUi(this);
}

PainterWidget::~PainterWidget()
{
    delete ui;
}
void PainterWidget::paint(QPainter *painter, QPaintEvent *event, int elapsed)
{

    QPen pen;
    pen.setColor(QColor(255,0,0));  //设置画笔为红色
    painter->setPen(pen);  //选择画笔

    DrawBackground(painter);
    printVeiw(painter);
}

void PainterWidget::DrawBackground(QPainter *painter)
{
    int width = this->width();
    int height = this->height();
    painter->fillRect(0,0,width,height,Qt::black);
    painter->setPen(Qt::yellow);
    for(int i=0;i<height/20;i++){
      painter->drawText(0,i*20,QString("%1").arg(height-i*20));
    }
}


void PainterWidget::DoDrawing(QPainter *painter, QList<TComplex*> *complex,FValueType type,const VeiwPara *veiwPara)
{
    DataMgr *pDataMgr = DataMgr::Inst();

    int bottom = veiwPara->CentralAxis;
    int PixIdx;
    int n, k;
    double SmpMin;
    double SmpMax;
    double SmpVal;
    int lastX = -1;
    int lastY = bottom-0;
    painter->setPen(veiwPara->Color);
    for(PixIdx=0; PixIdx<cPixPerFrame; PixIdx++){
    SmpMin = cMainImgHeight;
    SmpMax = 0;
    for(n=0; n<veiwPara->PixSmcnp; n++){
        if((pDataMgr->m_offset + PixIdx*veiwPara->PixSmcnp + n)>complex->size()-1)
            return;
        if(VALUE_R == type){
           SmpVal = complex->at(pDataMgr->m_offset + PixIdx*veiwPara->PixSmcnp + n)->R;
        }else if(VALUE_A == type){
           SmpVal = complex->at(pDataMgr->m_offset + PixIdx*veiwPara->PixSmcnp + n)->A;
        }else if(VALUE_P == type){
            if(F_P_P == veiwPara->VeiwID.ID){
                SmpVal = complex->at(pDataMgr->m_offset + PixIdx*veiwPara->PixSmcnp + n)->P;
            }else{
                SmpVal = complex->at(pDataMgr->m_offset + PixIdx*veiwPara->PixSmcnp + n)->P;
               // qDebug()<<"相位真实值："<<SmpVal;
               // SmpVal = (SmpVal*200/4)/PI;
            }
        }else if(VALUE_I == type){
           SmpVal = complex->at(pDataMgr->m_offset + PixIdx*veiwPara->PixSmcnp + n)->I;
        }

        SmpVal = SmpVal/veiwPara->K + veiwPara->C;
        if(SmpVal<SmpMin){
               SmpMin = SmpVal;
           }
        if(SmpVal>SmpMax){
                SmpMax = SmpVal;
           }
        }
        if(VALUE_P == type){

            if(F_P_P == veiwPara->VeiwID.ID){
               // if(SmpVal>veiwPara->C){
                    painter->drawLine(lastX,lastY,PixIdx,bottom-SmpMin);
                    painter->drawLine(PixIdx,bottom-SmpMin,PixIdx,bottom-SmpMax);
                    lastX = PixIdx;
                    lastY = bottom-SmpMax;
                //}
            }else{
               // if(SmpVal>veiwPara->C){
                    painter->drawPoint(PixIdx,bottom-100-SmpVal);
                    painter->drawPoint(PixIdx,bottom-SmpVal);
               // }
            }

        }else{
           // if(SmpMin>0){
                painter->drawLine(lastX,lastY,PixIdx,bottom-SmpMin);
                painter->drawLine(PixIdx,bottom-SmpMin,PixIdx,bottom-SmpMax);
                lastX = PixIdx;
                lastY = bottom-SmpMax;
           // }

            // qDebug()<<"bottom-SmpMax"<<SmpMax;
        }

    }
}

void PainterWidget::printVeiw(QPainter *painter)
{
    int width = this->width();
    int height = this->height();
    DataMgr *pDataMgr = DataMgr::Inst();
    QMapIterator<int,VeiwPara*> m(pDataMgr->m_ViewParaList);
    while (m.hasNext()) {
        m.next();
        VeiwPara* para = m.value();
        if(pDataMgr->IsLoadfile){
            switch (para->VeiwID.ID) {
            case T_CUR_A:
                DoDrawing(painter,&pDataMgr->m_VolCurT.curBA,VALUE_A,m.value());
                break;
            case T_CUR_R:
                DoDrawing(painter,&pDataMgr->m_VolCurT.curBA,VALUE_R,m.value());
                break;
            case T_CUR_I:
                DoDrawing(painter,&pDataMgr->m_VolCurT.curBA,VALUE_I,m.value());
                break;
            case T_CUR_P:
                DoDrawing(painter,&pDataMgr->m_VolCurT.curBA,VALUE_P,m.value());
                break;
            case T_LINE:
                painter->setPen(para->Color);
                painter->drawLine(0,para->CentralAxis,width,para->CentralAxis);
                break;
            case T_VOL_A:
                DoDrawing(painter,&pDataMgr->m_VolCurT.volBA,VALUE_A,m.value());
                break;
            case T_VOL_P:
                DoDrawing(painter,&pDataMgr->m_VolCurT.volBA,VALUE_P,m.value());
                break;
            case T_VOL_I:
                DoDrawing(painter,&pDataMgr->m_VolCurT.volBA,VALUE_I,m.value());
                break;
            case T_VOL_R:
                DoDrawing(painter,&pDataMgr->m_VolCurT.volBA,VALUE_R,m.value());
                break;
            case T_RE_A:
                DoDrawing(painter,&pDataMgr->m_VolCurT.ReBA,VALUE_A,m.value());
                break;
            case T_RE_P:
                DoDrawing(painter,&pDataMgr->m_VolCurT.ReBA,VALUE_P,m.value());
                break;
            case T_RE_R:
                DoDrawing(painter,&pDataMgr->m_VolCurT.ReBA,VALUE_R,m.value());
                break;
            case T_RE_I:
                DoDrawing(painter,&pDataMgr->m_VolCurT.ReBA,VALUE_I,m.value());
                break;
            case T_PID:
                    //DrawT(painter,&pDataMgr->m_PID[para->ID],m.value());
                break;
            case F_CUR_R:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.cur[para->Harm],VALUE_R,m.value());
                }
                break;
            case F_CUR_I:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.cur[para->Harm],VALUE_I,m.value());
                }
                break;
            case F_CUR_P:
                if(pDataMgr->IsCalc){
                    //qDebug()<<"F_CUR_P";
                    DoDrawing(painter,&pDataMgr->m_VolCurF.cur[para->Harm],VALUE_P,m.value());
                }
                break;
            case F_CUR_A:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.cur[para->Harm],VALUE_A,m.value());
                    qDebug()<<"F_CUR_A_Harm"<<para->viewName<<para->Harm;
                }
                break;
            case F_VOL_R:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.vol[para->Harm],VALUE_R,m.value());

                }
                break;
            case F_VOL_I:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.vol[para->Harm],VALUE_I,m.value());
                }
                break;
            case F_VOL_P:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.vol[para->Harm],VALUE_P,m.value());
                }
                break;
            case F_VOL_A:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.vol[para->Harm],VALUE_A,m.value());
                }
                break;
            case F_IMPED_R:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.imped[para->Harm],VALUE_P,m.value());
                }
                break;
            case F_IMPED_I:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.imped[para->Harm],VALUE_I,m.value());
                }
                break;
            case F_IMPED_P:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.imped[para->Harm],VALUE_P,m.value());
                }
                break;
            case F_IMPED_A:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_VolCurF.imped[para->Harm],VALUE_A,m.value());
                }
                break;
            case F_P_P:
                if(pDataMgr->IsCalc){
                    DoDrawing(painter,&pDataMgr->m_fpp[para->Harm],VALUE_P,m.value());
                }
                break;
            default:
                break;
            }
        }

    }
}

void PainterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, event, 1);
}

void PainterWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void PainterWidget::mousePressEvent(QMouseEvent *event)
{
    int height = this->height();
    DataMgr *pDataMgr = DataMgr::Inst();
    int x = event->x()+pDataMgr->m_offset;
    int y = height-event->y();
    QToolTip::showText(QCursor::pos(),QString("%1,%2").arg(x).arg(y));
}

void PainterWidget::resizeEvent(QResizeEvent *event)
{
   //this->repaint();
}
