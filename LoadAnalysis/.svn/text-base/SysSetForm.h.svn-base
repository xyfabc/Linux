#ifndef SYSSETFORM_H
#define SYSSETFORM_H

#include <QWidget>
#include "Pub.h"
#include "MListWidget.h"
#include "DataMgr.h"
#include <QMap>

namespace Ui {
class SysSetForm;
}

class SysSetForm : public QWidget
{
    Q_OBJECT

public:
    explicit SysSetForm(QWidget *parent = 0);
    ~SysSetForm();
    void init();
    void initListWidget();
//    void initCombox();
//    void SaveVeiwPara();
    void moveCurrentItem(MListWidget *source,
                                        MListWidget *target);
    int CreateID();
    void ReadCFG();
    void SaveCFG();
    QColor IntToQColor(const int &intColor);
    int  QColorToInt(const QColor &color);
    void DoFilter();
    QString GetNameBySel(QString sour, QString harm);

private slots:

    void on_addPb_clicked();

    void on_listWidget_1_currentRowChanged(int currentRow);



    void on_delPb_clicked();

    void on_paraSavePb_clicked();

    void on_colorTB_clicked();


    void on_RIAPCb_currentIndexChanged(int index);

    void on_harmCb_currentIndexChanged(int index);

    void on_dataSourComb_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    Ui::SysSetForm *ui;
    QMap<int,QListWidgetItem*> listItemMap;
    QString m_fileName;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SYSSETFORM_H
