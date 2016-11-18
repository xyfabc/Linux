#ifndef CALCCZTPARTTH_H
#define CALCCZTPARTTH_H

#include "Pub.h"

#include <QThread>

class CalcCZTPartTh : public QThread
{
    Q_OBJECT
public:
    CalcCZTPartTh();
    int pos;
protected:
     void run();

signals:
     void calcCZTpartOver();
};

#endif // CALCCZTPARTTH_H
