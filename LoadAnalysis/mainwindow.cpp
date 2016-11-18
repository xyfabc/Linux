#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <stdexcept>
MainWindow * MainWindow::m_self = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_self                 = this;
    createActions();
    createMenus();
    dataMgr = DataMgr::Inst();
    fileLoadProgS = new ProgressShow();
    calcCZTProgS = new ProgressShow();
    exportProgS = new ProgressShow();
    m_currentFile = NULL;
    sysSetForm = new SysSetForm();
    sysSetForm->init();
    connect(&exportFileTh,SIGNAL(updateMsg(int,QString)),exportProgS,SLOT(updateMsg(int,QString)),Qt::QueuedConnection);
    connect(dataMgr,SIGNAL(progressBarSignal(int,QString)),calcCZTProgS,SLOT(updateMsg(int,QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(updateMsg(int,QString)),fileLoadProgS,SLOT(updateMsg(int,QString)),Qt::QueuedConnection);
    connect(&loadFile,SIGNAL(runOver()),this,SLOT(LoadFileOverSlot()),Qt::QueuedConnection);

    connect(&calcCTZ,SIGNAL(runOver()),this,SLOT(calcCTZOverSlot()),Qt::QueuedConnection);
    connect(&calcCZTPartTh,SIGNAL(calcCZTpartOver()),this,SLOT(calcCZTpartOverSlot()),Qt::QueuedConnection);
    connect(dataMgr,SIGNAL(calcCZTpartOver()),this,SLOT(calcCZTpartOverSlot()));
}

MainWindow::~MainWindow()
{
    loadFile.quit();
    calcCTZ.quit();

    delete ui;
}
void MainWindow::openSlot()
{
    qDebug()<<"fileName";
    currentFileName = QFileDialog::getOpenFileName( MainWindow::m_self,"OpenFile","/","TXT (*.txt)");
    qDebug()<<currentFileName;
    if(currentFileName.isEmpty())
        return;
    if(!loadFile.isRunning()){

        loadFile.start();
        fileLoadProgS->raise();
         fileLoadProgS->show();
    }else{
       QMessageBox::information(this, "导入文件", "正在导入...");
    }
}

void MainWindow::createActions()
{
    //文件
    //openAct = new QAction(QIcon(Resouces::FileNewIcon), tr("打开"), this);
    openAct = new QAction(tr("载入数据"), this);
    openAct->setShortcut(tr("Ctrl+L"));
    openAct->setStatusTip(tr("载入电流电压数据"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openSlot()));

//    graphSetAct = new QAction(tr("显示设置"), this);
//    graphSetAct->setShortcut(tr("Ctrl+L"));
//    graphSetAct->setStatusTip(tr("显示设置"));
//    connect(graphSetAct, SIGNAL(triggered()), this, SLOT(graphSetSlot()));

    sysSetAct = new QAction(tr("参数设置"), this);
    sysSetAct->setShortcut(tr("Ctrl+L"));
    sysSetAct->setStatusTip(tr("参数设置"));
    connect(sysSetAct, SIGNAL(triggered()), this, SLOT(sysSetSlot()));

//    calcTAct = new QAction(tr("时域计算"), this);
//    calcTAct->setShortcut(tr("Ctrl+L"));
//    calcTAct->setStatusTip(tr("时域计算"));
//    connect(calcTAct, SIGNAL(triggered()), this, SLOT(calcTSlot()));

//    calcFAct = new QAction(tr("频域计算"), this);
//    calcFAct->setShortcut(tr("Ctrl+L"));
//    calcFAct->setStatusTip(tr("频域计算"));
//    connect(calcFAct, SIGNAL(triggered()), this, SLOT(calcFSlot()));

    exportFileAct = new QAction(tr("导出数据"), this);
    exportFileAct->setShortcut(tr("Ctrl+E"));
    exportFileAct->setStatusTip(tr("导出数据"));
    connect(exportFileAct, SIGNAL(triggered()), this, SLOT(exportFileSlot()));

}
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(exportFileAct);
//    caluMenu = menuBar()->addMenu(tr("计算(&C)"));
//    caluMenu->addAction(calcTAct);
//    caluMenu->addAction(calcFAct);
    sysMenu = menuBar()->addMenu(tr("系统(&S)"));
    sysMenu->addAction(sysSetAct);

//    graphMenu = menuBar()->addMenu(tr("显示(&G)"));
//    graphMenu->addAction(graphSetAct);
}
void MainWindow::RefFilePos(void)
{
    double ratio;
    if(m_currentFile != NULL){
        if(ui->sbarFilePosScrollBar->maximum()>0){
            ratio = ui->sbarFilePosScrollBar->value();
            ratio /= ui->sbarFilePosScrollBar->maximum();
        }else{
            ratio = 0;
        }
        ui->sbarFilePosScrollBar->setMaximum(0);
        ui->sbarFilePosScrollBar->setMaximum(dataMgr->m_VolCurT.curBA.size()/cPixPerFrame);

        ratio *= ui->sbarFilePosScrollBar->maximum();
        ui->sbarFilePosScrollBar->setValue(ratio);
        ui->label->setText(QString("%1/%2").arg(ratio).arg(ui->sbarFilePosScrollBar->maximum()));

    }
}

void MainWindow::LoadFile()
{
    dataMgr->IsLoadfile = false;
    dataMgr->m_VolCurT.volBA.clear();
    dataMgr->m_VolCurT.curBA.clear();

    bool isAgain = true;  //
    TCplxFp vol ;
    TCplxFp cur ;
    int lastCount = 255;
    m_currentFile = new QFile(currentFileName);
    int size = m_currentFile->size();
    if (!m_currentFile->open(QIODevice::ReadOnly))
             return;
    QTextStream in(m_currentFile);
    int i = 0;
    while (!in.atEnd()) {
              QString dataLine = in.readLine();
              int cout = dataLine.section("	", 2, 2).toInt();
              QString flag = dataLine.section("	", 3, 3);
              TComplex *val_cur = new TComplex;
              TComplex *val_vol = new TComplex;
              if(!isAgain){
                  if("LOST!" ==  flag){
                      for(int i=1;i< (cout-lastCount);i++)
                      {
                          val_cur->R = cur;
                          val_cur->I = 0;
                          val_vol->R = vol;
                          val_vol->I = 0;
                          dataMgr->m_VolCurT.volBA.append(val_vol);
                          dataMgr->m_VolCurT.curBA.append(val_cur);
                      }
                  }
              }
              vol = dataLine.section("	", 0, 0).toFloat();
              cur = dataLine.section("	", 1, 1).toFloat();
            // qDebug()<<vol<<":"<<cur<<":"<<cout<<":" <<flag<<":";
             val_cur->R = cur;
             val_cur->I = 0;
             val_vol->R = vol;
             val_vol->I = 0;
             if(0 == i%dataMgr->m_CZTPara.aLimit){
                 dataMgr->m_VolCurT.volBA.append(val_vol);
                 dataMgr->m_VolCurT.curBA.append(val_cur);
             }

             // qDebug()<<"---------------------------------"<<vol<<":"<<cur;
              if(cout == 255){
               isAgain = true;
              }else{
               isAgain = false;
              }
              lastCount = cout;
              i++;
              double t = (double)i*14.88/(double)size*100;
              emit updateMsg(t,"正在载入电流电压...");
              if(i>FileMaxLength){
                  break;
              }
          }
    dataMgr->m_size = i;
    //qDebug()<<"the lenth = "<<i;


    RefFilePos();
    dataMgr->initCTZ();
    dataMgr->CalcCZT(ui->sbarFilePosScrollBar->value());
    dataMgr->CalcRe();

    m_currentFile->close();
    dataMgr->IsLoadfile = true;
}

void MainWindow::on_sbarFilePosScrollBar_valueChanged(int value)
{
    qDebug()<<"on_sbarFilePosScrollBar_valueChanged";
    ui->label->setText(QString("%1/%2").arg(value).arg(ui->sbarFilePosScrollBar->maximum()));
    if(dataMgr->m_VolCurT.curBA.size()>0){
        int offset = int(ui->sbarFilePosScrollBar->value()*cPixPerFrame);
        dataMgr->m_offset = offset;
        dataMgr->isPosChange = true;
        if(dataMgr->IsLoadfile){
            if(calcCZTPartTh.isRunning()){
                calcCZTPartTh.quit();
            }
            calcCZTPartTh.pos = ui->sbarFilePosScrollBar->value();
            calcCZTPartTh.start();
        }
        ui->widget->repaint();
    }
}

void MainWindow::LoadFileOverSlot()
{
    fileLoadProgS->hide();
}

void MainWindow::calcCTZOverSlot()
{
    calcCZTProgS->hide();
}

void MainWindow::calcCZTpartOverSlot()
{
    ui->widget->repaint();
}

void MainWindow::exportFileSlot()
{
    QString fileName =QFileDialog::	getSaveFileName(0,
                                                  ("导出数据"),
                                                  "/",
                                                  ("excel(*.xls *.xlsx)"));
    if (fileName.isEmpty()) {
    QMessageBox::critical(0,("错误"), ("要导出的文件名为空！"));
    return ;
    }
    QFile *file = new QFile(fileName);
    if(!file->exists()){
    return;
    }
    exportFileTh.m_fileName = fileName;
    if(!exportFileTh.isRunning()){
        exportFileTh.start();
    }else{
        QMessageBox::information(0, "系统提示", "正在导出，请稍候!");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
     sysSetForm->close();
}
void MainWindow::sysSetSlot()
{
    sysSetForm->init();
    sysSetForm->showNormal();
}





