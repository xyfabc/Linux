#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QPaintEvent>
#include "Pub.h"
#include <QToolTip>

namespace Ui {
class PainterWidget;
}

typedef enum{
    VALUE_R,
    VALUE_I,
    VALUE_A,
    VALUE_P,
    VALUE_P_P,
}FValueType;

class PainterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PainterWidget(QWidget *parent = 0);
    ~PainterWidget();
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);     //
    void DrawBackground(QPainter *painter);                             //画背景   
    void DoDrawing(QPainter *painter, QList<TComplex *> *complex, FValueType type, const VeiwPara *veiwPara);
    void printVeiw(QPainter *painter);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void resizeEvent (QResizeEvent * event );

private:
    Ui::PainterWidget *ui;
};

#endif // PAINTERWIDGET_H
