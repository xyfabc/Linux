#ifndef LOADFILE_H
#define LOADFILE_H

#include <QThread>

class LoadFileTh : public QThread
{
    Q_OBJECT
public:
    LoadFileTh();
    QString fileName;

protected:
     void run();
signals:
     void runOver();

};

#endif // LOADFILE_H
