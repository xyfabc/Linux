#include "ProgressShow.h"
#include "ui_ProgressShow.h"
#include <QDebug>

ProgressShow::ProgressShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressShow)
{
    ui->setupUi(this);
     this->setWindowTitle("系统提示");
    setWindowFlags ( Qt::WindowTitleHint);
}

ProgressShow::~ProgressShow()
{
    delete ui;
}


void ProgressShow::updateMsg(int val, QString msg)
{
    if(val>0){
        this->show();
    }
   ui->progressBar->setValue(val);
   ui->label->setText(msg);
   if(100 == val){
       this->hide();
   }
}
