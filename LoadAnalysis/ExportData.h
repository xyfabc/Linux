#ifndef ExportData_H
#define ExportData_H

#include "Excel.h"
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include "DataMgr.h"


class ExportData : public QThread
{
    Q_OBJECT
public:
    ExportData();
    void writeData();
    ~ExportData();
     QString m_fileName;

private:
     QExcel *m_excel;

     //ProgressShow *exportProgS;

signals:
    void updateMsg(int,QString);
    void progressSwitch(bool);

protected:
     void run();

};

#endif // ExportData_H
