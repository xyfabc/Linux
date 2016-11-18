/********************************************************************************
** Form generated from reading UI file 'SysSetForm.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYSSETFORM_H
#define UI_SYSSETFORM_H

#include <MListWidget.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SysSetForm
{
public:
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QPushButton *pushButton;
    QLabel *label_5;
    QLineEdit *secEdi;
    QLineEdit *pixEdi;
    QLabel *label_6;
    QLineEdit *limEdi;
    QLabel *label_7;
    QLineEdit *samRateEdi;
    QLabel *label_8;
    QWidget *tab_3;
    MListWidget *listWidget;
    QComboBox *dataSourComb;
    QLabel *label_11;
    MListWidget *listWidget_1;
    QPushButton *addPb;
    QPushButton *delPb;
    QLabel *label_12;
    QPushButton *paraSavePb;
    QLabel *harmLB;
    QComboBox *harmCb;
    QLabel *harmLB_2;
    QComboBox *RIAPCb;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLineEdit *veiw_kEdi;
    QLabel *CentralAxisLB;
    QLineEdit *CentralAxisEdit;
    QLabel *label;
    QLabel *pixLB;
    QPushButton *colorTB;
    QLabel *CLB;
    QLabel *KLB;
    QLineEdit *view_cEdi;
    QLineEdit *pixSmcnpEdi;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *SysSetForm)
    {
        if (SysSetForm->objectName().isEmpty())
            SysSetForm->setObjectName(QStringLiteral("SysSetForm"));
        SysSetForm->resize(552, 623);
        tabWidget = new QTabWidget(SysSetForm);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 531, 601));
        tabWidget->setMinimumSize(QSize(531, 601));
        tabWidget->setMaximumSize(QSize(531, 601));
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        pushButton = new QPushButton(tab_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(180, 140, 75, 23));
        label_5 = new QLabel(tab_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 40, 61, 20));
        secEdi = new QLineEdit(tab_2);
        secEdi->setObjectName(QStringLiteral("secEdi"));
        secEdi->setGeometry(QRect(90, 40, 91, 20));
        pixEdi = new QLineEdit(tab_2);
        pixEdi->setObjectName(QStringLiteral("pixEdi"));
        pixEdi->setGeometry(QRect(370, 40, 91, 20));
        label_6 = new QLabel(tab_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(300, 40, 71, 21));
        limEdi = new QLineEdit(tab_2);
        limEdi->setObjectName(QStringLiteral("limEdi"));
        limEdi->setGeometry(QRect(370, 80, 91, 20));
        label_7 = new QLabel(tab_2);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(300, 80, 71, 20));
        samRateEdi = new QLineEdit(tab_2);
        samRateEdi->setObjectName(QStringLiteral("samRateEdi"));
        samRateEdi->setGeometry(QRect(90, 80, 91, 20));
        label_8 = new QLabel(tab_2);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 80, 61, 20));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        listWidget = new MListWidget(tab_3);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(10, 100, 201, 321));
        dataSourComb = new QComboBox(tab_3);
        dataSourComb->setObjectName(QStringLiteral("dataSourComb"));
        dataSourComb->setGeometry(QRect(90, 10, 121, 22));
        label_11 = new QLabel(tab_3);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(20, 10, 61, 16));
        listWidget_1 = new MListWidget(tab_3);
        listWidget_1->setObjectName(QStringLiteral("listWidget_1"));
        listWidget_1->setGeometry(QRect(300, 100, 211, 321));
        addPb = new QPushButton(tab_3);
        addPb->setObjectName(QStringLiteral("addPb"));
        addPb->setGeometry(QRect(240, 190, 41, 31));
        QIcon icon;
        icon.addFile(QStringLiteral(":/new/prefix1/images/rightarrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        addPb->setIcon(icon);
        delPb = new QPushButton(tab_3);
        delPb->setObjectName(QStringLiteral("delPb"));
        delPb->setGeometry(QRect(240, 290, 41, 31));
        label_12 = new QLabel(tab_3);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(300, 70, 61, 16));
        paraSavePb = new QPushButton(tab_3);
        paraSavePb->setObjectName(QStringLiteral("paraSavePb"));
        paraSavePb->setGeometry(QRect(210, 540, 75, 23));
        harmLB = new QLabel(tab_3);
        harmLB->setObjectName(QStringLiteral("harmLB"));
        harmLB->setGeometry(QRect(20, 40, 61, 20));
        harmCb = new QComboBox(tab_3);
        harmCb->setObjectName(QStringLiteral("harmCb"));
        harmCb->setGeometry(QRect(90, 40, 121, 22));
        harmLB_2 = new QLabel(tab_3);
        harmLB_2->setObjectName(QStringLiteral("harmLB_2"));
        harmLB_2->setGeometry(QRect(20, 70, 61, 20));
        RIAPCb = new QComboBox(tab_3);
        RIAPCb->setObjectName(QStringLiteral("RIAPCb"));
        RIAPCb->setGeometry(QRect(90, 70, 121, 22));
        groupBox = new QGroupBox(tab_3);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 450, 501, 81));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        veiw_kEdi = new QLineEdit(groupBox);
        veiw_kEdi->setObjectName(QStringLiteral("veiw_kEdi"));

        gridLayout->addWidget(veiw_kEdi, 0, 4, 1, 1);

        CentralAxisLB = new QLabel(groupBox);
        CentralAxisLB->setObjectName(QStringLiteral("CentralAxisLB"));

        gridLayout->addWidget(CentralAxisLB, 1, 0, 1, 1);

        CentralAxisEdit = new QLineEdit(groupBox);
        CentralAxisEdit->setObjectName(QStringLiteral("CentralAxisEdit"));

        gridLayout->addWidget(CentralAxisEdit, 1, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 6, 1, 1);

        pixLB = new QLabel(groupBox);
        pixLB->setObjectName(QStringLiteral("pixLB"));

        gridLayout->addWidget(pixLB, 0, 0, 1, 1);

        colorTB = new QPushButton(groupBox);
        colorTB->setObjectName(QStringLiteral("colorTB"));
        colorTB->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(colorTB, 1, 7, 1, 1);

        CLB = new QLabel(groupBox);
        CLB->setObjectName(QStringLiteral("CLB"));

        gridLayout->addWidget(CLB, 0, 6, 1, 1);

        KLB = new QLabel(groupBox);
        KLB->setObjectName(QStringLiteral("KLB"));

        gridLayout->addWidget(KLB, 0, 3, 1, 1);

        view_cEdi = new QLineEdit(groupBox);
        view_cEdi->setObjectName(QStringLiteral("view_cEdi"));

        gridLayout->addWidget(view_cEdi, 0, 7, 1, 1);

        pixSmcnpEdi = new QLineEdit(groupBox);
        pixSmcnpEdi->setObjectName(QStringLiteral("pixSmcnpEdi"));

        gridLayout->addWidget(pixSmcnpEdi, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        tabWidget->addTab(tab_3, QString());
        dataSourComb->raise();
        label_11->raise();
        listWidget_1->raise();
        addPb->raise();
        delPb->raise();
        label_12->raise();
        paraSavePb->raise();
        listWidget->raise();
        harmLB->raise();
        harmCb->raise();
        harmLB_2->raise();
        RIAPCb->raise();
        groupBox->raise();

        retranslateUi(SysSetForm);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(SysSetForm);
    } // setupUi

    void retranslateUi(QWidget *SysSetForm)
    {
        SysSetForm->setWindowTitle(QApplication::translate("SysSetForm", "\345\217\202\346\225\260\350\256\276\347\275\256", 0));
        pushButton->setText(QApplication::translate("SysSetForm", "\346\233\264\346\224\271", 0));
        label_5->setText(QApplication::translate("SysSetForm", "\346\256\265 \351\207\207 \346\240\267\357\274\232", 0));
        secEdi->setText(QString());
        pixEdi->setText(QString());
        label_6->setText(QApplication::translate("SysSetForm", "\350\256\241\347\256\227\345\203\217\351\207\207\346\240\267\357\274\232", 0));
        limEdi->setText(QString());
        label_7->setText(QApplication::translate("SysSetForm", "\347\274\251 \345\260\217 \345\200\215\346\225\260\357\274\232", 0));
        samRateEdi->setText(QString());
        label_8->setText(QApplication::translate("SysSetForm", "\345\216\237\351\207\207\346\240\267\347\216\207\357\274\232", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SysSetForm", "\351\242\221\345\237\237\345\217\202\346\225\260", 0));
        dataSourComb->clear();
        dataSourComb->insertItems(0, QStringList()
         << QApplication::translate("SysSetForm", "\347\224\265\346\265\201", 0)
         << QApplication::translate("SysSetForm", "\347\224\265\345\216\213", 0)
         << QApplication::translate("SysSetForm", "\351\230\273\346\212\227", 0)
         << QApplication::translate("SysSetForm", "\347\272\277", 0)
        );
        label_11->setText(QApplication::translate("SysSetForm", "\346\225\260 \346\215\256 \346\272\220\357\274\232", 0));
        addPb->setText(QString());
        delPb->setText(QApplication::translate("SysSetForm", "\345\210\240\351\231\244", 0));
        label_12->setText(QApplication::translate("SysSetForm", "\345\267\262\351\200\211\345\210\227\350\241\250\357\274\232", 0));
        paraSavePb->setText(QApplication::translate("SysSetForm", "\344\277\235\345\255\230\345\217\202\346\225\260", 0));
        harmLB->setText(QApplication::translate("SysSetForm", "\350\260\220\346\263\242\345\210\206\351\207\217\357\274\232", 0));
        harmCb->clear();
        harmCb->insertItems(0, QStringList()
         << QApplication::translate("SysSetForm", "\345\237\272\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\344\272\214\346\254\241\350\260\220\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\344\270\211\346\254\241\350\260\220\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\345\233\233\346\254\241\350\260\220\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\344\272\224\346\254\241\350\260\220\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\345\205\255\346\254\241\350\260\220\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\344\270\203\346\254\241\350\260\220\346\263\242", 0)
         << QApplication::translate("SysSetForm", "\346\227\266\345\237\237", 0)
        );
        harmLB_2->setText(QApplication::translate("SysSetForm", "\345\210\206\351\207\217\347\261\273\345\236\213\357\274\232", 0));
        RIAPCb->clear();
        RIAPCb->insertItems(0, QStringList()
         << QApplication::translate("SysSetForm", "\345\256\236\351\203\250", 0)
         << QApplication::translate("SysSetForm", "\350\231\232\351\203\250", 0)
         << QApplication::translate("SysSetForm", "\347\233\270\344\275\215", 0)
         << QApplication::translate("SysSetForm", "\346\250\241\345\200\274", 0)
        );
        groupBox->setTitle(QString());
        CentralAxisLB->setText(QApplication::translate("SysSetForm", "\345\272\225    \350\275\264\357\274\232", 0));
        label->setText(QApplication::translate("SysSetForm", "\351\200\211\346\213\251\351\242\234\350\211\262\357\274\232", 0));
        pixLB->setText(QApplication::translate("SysSetForm", "\345\203\217 \351\207\207 \346\240\267\357\274\232", 0));
        colorTB->setText(QString());
        CLB->setText(QApplication::translate("SysSetForm", "\344\270\212\344\270\213\344\275\215\347\247\273\357\274\232", 0));
        KLB->setText(QApplication::translate("SysSetForm", "\344\270\212\344\270\213\347\274\251\346\224\276\357\274\232", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("SysSetForm", "\346\230\276\347\244\272\350\256\276\347\275\256", 0));
    } // retranslateUi

};

namespace Ui {
    class SysSetForm: public Ui_SysSetForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYSSETFORM_H
