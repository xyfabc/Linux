#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PainterWidget.h"
#include "DataMgr.h"
#include "LoadFile.h"
#include "ProgressShow.h"
#include "SysSetForm.h"
#include "CalcCTZ.h"
#include <QDomDocument>
#include <QTextCodec>
#include "CalcCZTPartTh.h"
#include "ExportData.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createActions();
    void createMenus();
    void RefFilePos(void);
public:
    static MainWindow      *m_self;
    void LoadFile();

private slots:
    void openSlot();
    void on_sbarFilePosScrollBar_valueChanged(int value);
//    void graphSetSlot();
    void sysSetSlot();
//    void calcTSlot();
//    void calcFSlot();

private:
    Ui::MainWindow *ui;
    QMenu *fileMenu;                //文件菜单
    QMenu *caluMenu;                //计算菜单
    QMenu *sysMenu;                 //系统菜单
    QMenu *graphMenu;                //显示菜单
    QAction *openAct;
    QAction *graphSetAct;
    QAction *sysSetAct;
    QAction *calcTAct;
    QAction *calcFAct;
    QAction *exportFileAct;
    DataMgr *dataMgr;
    QFile *m_currentFile;
    QString currentFileName;

    LoadFileTh loadFile;
    ProgressShow *fileLoadProgS;
    ProgressShow *calcCZTProgS;
    ProgressShow *exportProgS;
    SysSetForm *sysSetForm;
    CalcCTZTh calcCTZ;
    CalcCZTPartTh calcCZTPartTh;
    ExportData exportFileTh;

public slots:
//    void progressBarSlot(int);
    void LoadFileOverSlot();
    void calcCTZOverSlot();
    void calcCZTpartOverSlot();
    void exportFileSlot();
signals:
    void updateMsg(int,QString);
    void progressSwitch(bool);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
