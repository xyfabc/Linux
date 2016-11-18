#ifndef CALCCTZ_H
#define CALCCTZ_H

#include <QThread>

class CalcCTZTh : public QThread
{
    Q_OBJECT
public:
    CalcCTZTh();
protected:
     void run();
signals:
     void runOver();
};

#endif // CALCCTZ_H
