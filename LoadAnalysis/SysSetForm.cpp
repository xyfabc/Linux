#include "SysSetForm.h"
#include "ui_SysSetForm.h"
#include <QColorDialog>
#include <QPalette>
#include <QDomDocument>
#include <QTextCodec>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

SysSetForm::SysSetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SysSetForm)
{
    ui->setupUi(this);
    ui->listWidget->IsCanAdd = false;
    ui->listWidget->IsCanDel = false;
    ui->dataSourComb->setCurrentIndex(-1);
    ui->listWidget_1->setCurrentRow(-1);
}

SysSetForm::~SysSetForm()
{
    delete ui;
}

void SysSetForm::init()
{
    ReadCFG();
    ui->dataSourComb->setCurrentIndex(0);
    initListWidget();
    DataMgr *pDataMgr = DataMgr::Inst();
    ui->secEdi->setText(QString("%1").arg(pDataMgr->m_CZTPara.sectSmpCnt));
    ui->pixEdi->setText(QString("%1").arg(pDataMgr->m_CZTPara.pixSmcnt));
    ui->limEdi->setText(QString("%1").arg(pDataMgr->m_CZTPara.aLimit));
    ui->samRateEdi->setText(QString("%1").arg(pDataMgr->m_CZTPara.smRate));
}

void SysSetForm::initListWidget()
{
    ui->listWidget_1->setCurrentRow(-1);
    ui->listWidget_1->clear();
    listItemMap.clear();
    DataMgr *pDataMgr = DataMgr::Inst();
    QMapIterator<int,VeiwPara*> m(pDataMgr->m_ViewParaList);
    while (m.hasNext()) {
        m.next();
        VeiwPara* para = m.value();
        int id = para->ID;
        QListWidgetItem *newItem = new QListWidgetItem(para->viewName);
        listItemMap.insert(id,newItem);
        ui->listWidget_1->addItem(newItem);
        ui->listWidget_1->setCurrentRow(id);
    }

}

void SysSetForm::on_addPb_clicked()
{
    if(ui->listWidget->currentItem()){
        moveCurrentItem(ui->listWidget, ui->listWidget_1);
    }
}

void SysSetForm::moveCurrentItem(MListWidget *source,
                                    MListWidget *target)
{
    if (source->currentItem()) {
        QListWidgetItem *newItem = source->currentItem()->clone();
        DataMgr *pDataMgr = DataMgr::Inst();
        VeiwPara *para = new VeiwPara;
        para->init();
        int id = CreateID();
        para->VeiwID.name = ui->listWidget->currentItem()->text();
        if(newItem->text() == "线"){
           para->viewName =para->VeiwID.name;
        }else{
           para->viewName = GetNameBySel(para->VeiwID.name,ui->harmCb->currentText());
        }
        newItem->setText(para->viewName);
        para->Harm = ui->harmCb->currentIndex();
        qDebug()<<"moveCurrentItem_Harm"<<para->Harm;
        listItemMap.insert(id,newItem);

        para->VeiwID.ID = pDataMgr->GetViewIdByName(ui->listWidget->currentItem()->text());     
        para->ID = id;
        para->VeiwID.dataType = pDataMgr->m_ViewList[para->VeiwID.ID]->dataType;
        pDataMgr->m_ViewParaList.insert(id,para);
        target->addItem(newItem);
        target->setCurrentItem(newItem);
    }
}

int SysSetForm::CreateID()
{
    int value=0;
    bool flag;
    while(1)
    {
        flag = false;
        QMapIterator<int,QListWidgetItem*> i(listItemMap);
        while (i.hasNext()) {
            i.next();
            if(i.key() == value){
                flag = true;
            }
        }
        if(!flag){
            return value;
        }
        value++;
    }
    return -1;
}

void SysSetForm::ReadCFG()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

        QFile *file1;
        QString  filename = "config.xml";
        if(file1->exists(filename))
        {
            qDebug() <<"exist" ;

            QDomDocument doc("mydocument");
            QFile file("./config.xml");
            if (!file.open(QIODevice::ReadWrite)){
                qDebug() <<"open fail" ;
                       return;
            }
            if (!doc.setContent(&file)) {
                file.close();
            }
            QDomElement root = doc.documentElement();
            if (root.tagName() != "LoadAnalysisCfg") {
               qDebug()<<"root.tagname != ipconfig..." ;
            }
            DataMgr *pDataMgr = DataMgr::Inst();
            pDataMgr->m_ViewParaList.clear();
            QDomNode n = root.firstChild();
            QDomElement e = n.toElement();
            while(!n.isNull()) {
                QDomElement e = n.toElement(); // try to convert the node to an element.
                if(!e.isNull()) {
                    qDebug() <<"NODE " <<e.tagName()
                            << e.attribute("ID")
                            << e.attribute("C")
                            << e.attribute("CentralAxis")
                            << e.attribute("Harm")
                            << e.attribute("K")
                            << e.attribute("PixSmcnp")
                            << e.attribute("Size")
                            << e.attribute("Color")
                            << e.attribute("VeiwTypeID")
                            << e.attribute("VeiwDomaType")
                            << e.attribute("VeiwName")
                            << e.attribute("PIDVeiwTypeID")
                            << e.attribute("PIDp1")
                            << e.attribute("PIDp2")
                            << e.attribute("PIDIDType"); // the node really is an element.
                 VeiwPara *veiwPara = new VeiwPara();
                 veiwPara->ID               = e.attribute("ID").toInt();
                 veiwPara->C                = e.attribute("C").toInt();
                 veiwPara->CentralAxis      = e.attribute("CentralAxis").toInt();
                 veiwPara->Harm             = e.attribute("Harm").toInt();
                 veiwPara->K                = e.attribute("K").toFloat();
                 veiwPara->PixSmcnp         = e.attribute("PixSmcnp").toInt();
                 veiwPara->Size             = e.attribute("Size").toInt();
                 veiwPara->Color            = IntToQColor(e.attribute("Color").toInt());
                 veiwPara->VeiwID.ID        = (VeiwTypeID)e.attribute("VeiwTypeID").toInt();
                 veiwPara->VeiwID.Type      = (VeiwDomaType)e.attribute("VeiwDomaType").toInt();
                 veiwPara->VeiwID.name      = e.attribute("VeiwSourName");
                 veiwPara->viewName         = e.attribute("viewName");
                 veiwPara->VeiwID.dataType  = (DataType)e.attribute("dataType").toInt();

//                 veiwPara->pidPara.PIDSour  = (VeiwTypeID)e.attribute("PIDVeiwTypeID").toInt();
//                 veiwPara->pidPara.mid      = e.attribute("PIDMid").toInt();
//                 veiwPara->pidPara.p1       = e.attribute("PIDp1").toFloat();
//                 veiwPara->pidPara.p2       = e.attribute("PIDp2").toFloat();
                 pDataMgr->m_ViewParaList.insert(veiwPara->ID,veiwPara);

                }
                n = n.nextSibling();
            }
           file.close();
        }
        else
        {
            //不存在
        }
}

void SysSetForm::SaveCFG()
{
    QDomDocument doc("mydocument");
    QFile::remove("./config.xml");
    QFile file("./config.xml");
    if (!file.open(QIODevice::ReadWrite)){
               qDebug() <<"open" ;
         return;
    }

    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"GB2312\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("LoadAnalysisCfg");
    doc.appendChild(root);
    doc.appendChild(root);
    QDomText text = doc.createTextNode("");
    root.appendChild(text);

    DataMgr *pDataMgr = DataMgr::Inst();
    QMapIterator<int,VeiwPara*> m(pDataMgr->m_ViewParaList);
    while (m.hasNext()) {
        m.next();
        VeiwPara* para = m.value();

         QDomElement elem = doc.createElement("viewItem");
         elem.setAttribute("ID",            QString("%1").arg(para->ID));
         elem.setAttribute("C",             QString("%1").arg(para->C));
         elem.setAttribute("CentralAxis",   QString("%1").arg(para->CentralAxis));
         elem.setAttribute("Harm",          QString("%1").arg(para->Harm));
         elem.setAttribute("K",             QString("%1").arg(para->K));
         elem.setAttribute("PixSmcnp",      QString("%1").arg(para->PixSmcnp));
         elem.setAttribute("Size",          QString("%1").arg(para->Size));
         elem.setAttribute("Color",         QString("%1").arg(QColorToInt(para->Color)));
         elem.setAttribute("VeiwTypeID",    QString("%1").arg(para->VeiwID.ID));
         elem.setAttribute("VeiwDomaType",  QString("%1").arg(para->VeiwID.Type));
         elem.setAttribute("VeiwSourName",  QString("%1").arg(para->VeiwID.name));
         elem.setAttribute("viewName",      QString("%1").arg(para->viewName));
         elem.setAttribute("dataType",      QString("%1").arg(para->VeiwID.dataType));
         qDebug()<<"viewName"<<para->viewName;
//         elem.setAttribute("PIDVeiwTypeID", QString("%1").arg(para->pidPara.PIDSour));
//         elem.setAttribute("PIDIDType",  QString("%1").arg(para->pidPara.IDType));
//         elem.setAttribute("PIDMid",  QString("%1").arg(para->pidPara.mid));
//         elem.setAttribute("PIDp1",  QString("%1").arg(para->pidPara.p1));
//         elem.setAttribute("PIDp2",  QString("%1").arg(para->pidPara.p2));
         root.appendChild(elem);
    }
    QString xml = doc.toString();
    QTextStream out(&file);
    out<<xml;
    file.close();
}

void SysSetForm::on_listWidget_1_currentRowChanged(int currentRow)
{
    if(ui->listWidget_1->currentRow()<0){
       return;
    }
    DataMgr *pDataMgr = DataMgr::Inst();
    int id = listItemMap.key(ui->listWidget_1->currentItem());
    VeiwPara *para = pDataMgr->m_ViewParaList.value(id);
    if(para != NULL){
           switch (para->VeiwID.dataType) {
                          qDebug()<<"T_DOM_LINE"<<para->VeiwID.dataType;
           case DATA_TYPE_CUR:
           case DATA_TYPE_VOL:
           case DATA_TYPE_IMPED:

               ui->pixLB->setVisible(true);
               ui->pixSmcnpEdi->setVisible(true);
               ui->KLB->setVisible(true);
               ui->CLB->setVisible(true);
               ui->veiw_kEdi->setVisible(true);
               ui->view_cEdi->setVisible(true);
               ui->veiw_kEdi->setText(QString("%1").arg(para->K));
               ui->view_cEdi->setText(QString("%1").arg(para->C));
               ui->CentralAxisEdit->setText(QString("%1").arg(para->CentralAxis));
               //ui->harmCb->setCurrentIndex(para->Harm);
               ui->pixSmcnpEdi->setText(QString("%1").arg(para->PixSmcnp));
                   break;
           case DATA_TYPE_LINE:
               qDebug()<<"T_DOM_LINE";
               ui->pixLB->setVisible(false);
               ui->pixSmcnpEdi->setVisible(false);
               ui->KLB->setVisible(false);
               ui->CLB->setVisible(false);
               ui->veiw_kEdi->setVisible(false);
               ui->view_cEdi->setVisible(false);
               break;
//           case T_DOM_PID:
//               ui->pixLB->setEnabled(true);
//               ui->pixSmcnpEdi->setEnabled(true);
////               ui->viewSizeLB->setEnabled(true);
////               ui->veiw_sizeEdit->setEnabled(true);
//               ui->KLB->setEnabled(true);
//               ui->CLB->setEnabled(true);
//               ui->veiw_kEdi->setEnabled(true);
//               ui->view_cEdi->setEnabled(true);

//               ui->p1Edi->setText(QString("%1").arg(para->pidPara.p1));
//               ui->p2Edi->setText(QString("%1").arg(para->pidPara.p2));
//               ui->pidCmb->setCurrentIndex(para->pidPara.PIDSour);
//               ui->idTypeCmd->setCurrentIndex(para->pidPara.IDType);
//               ui->midEdi->setText(QString("%1").arg(para->pidPara.mid));

           //    break;
           default:
               break;
           }
        ui->veiw_kEdi->setText(QString("%1").arg(para->K));
        ui->view_cEdi->setText(QString("%1").arg(para->C));
        ui->CentralAxisEdit->setText(QString("%1").arg(para->CentralAxis));
        ui->pixSmcnpEdi->setText(QString("%1").arg(para->PixSmcnp));
        QPalette   pal   =   ui->colorTB->palette();
        pal.setColor(QPalette::Button,para->Color);
        ui->colorTB->setPalette(pal);

    }
}


void SysSetForm::on_delPb_clicked()
{
    DataMgr *pDataMgr = DataMgr::Inst();
    if(ui->listWidget_1->currentRow()>=0){
        int id = listItemMap.key(ui->listWidget_1->currentItem());
        pDataMgr->m_ViewParaList.remove(id);
        listItemMap.remove(id);
        delete ui->listWidget_1->currentItem();
        qDebug()<<"removeID = "<<id;
    }
}

void SysSetForm::on_paraSavePb_clicked()
{
    if(ui->listWidget_1->currentRow()<0){
        return;
    }
    DataMgr *pDataMgr = DataMgr::Inst();
    int id = listItemMap.key(ui->listWidget_1->currentItem());

    VeiwPara *para = pDataMgr->m_ViewParaList.value(id);
    if(para != NULL){
        para->K = ui->veiw_kEdi->text().toFloat();
        para->C = ui->view_cEdi->text().toInt();
        para->CentralAxis = ui->CentralAxisEdit->text().toInt();
       // para->Harm = ui->harmCb->currentIndex();
        para->PixSmcnp = ui->pixSmcnpEdi->text().toInt();
        QColor color = ui->colorTB->palette().color(QPalette::Button);
        para->Color = color;
    }
}

void SysSetForm::on_colorTB_clicked()
{
    QColor color = ui->colorTB->palette().color(QPalette::Button);
    color        = QColorDialog::getColor(color, NULL);
    QPalette   pal   =   ui->colorTB->palette();
    pal.setColor(QPalette::Button,color);
    ui->colorTB->setPalette(pal);
}

void SysSetForm::closeEvent(QCloseEvent *event)
{
    qDebug()<<"closeEvent";
SaveCFG();
}

int  SysSetForm::QColorToInt(const QColor &color)
{

    return  (int)(((unsigned int)color.blue()<< 16) | (unsigned short)(((unsigned short)color.green()<< 8) | color.red()));
}

void SysSetForm::DoFilter()
{
    ui->listWidget->clear();
    DataMgr *pDataMgr = DataMgr::Inst();
    QMapIterator<int,VeiwSourPara*> m(pDataMgr->m_ViewList);
    while (m.hasNext()) {
        m.next();
        VeiwSourPara* para = m.value();
        if(para->dataType == ui->dataSourComb->currentIndex()){
            if(para->ripaType == ui->RIAPCb->currentIndex()){
                if(7 == ui->harmCb->currentIndex()){   //显示时域
                    if(para->ID<12){  //时域
                      ui->listWidget->addItem(para->name);
                    }

                }else{                  //频域
                    if(para->ID>11){    //频域
                      ui->listWidget->addItem(para->name);
                      qDebug()<<"selec"<<para->name;
                    }
                }

            }
            if(DATA_TYPE_LINE == para->dataType){   //线
                ui->listWidget->addItem(para->name);
            }
            if(RIPA_TYPE_A_A == para->ripaType){   //线
                 if(para->dataType == ui->dataSourComb->currentIndex()){
                     ui->listWidget->addItem(para->name);
                 }
            }

        }
    }

}

QString SysSetForm::GetNameBySel(QString sour,QString harm)
{
   QString name = sour+"("+harm+")";
   return name;
}
QColor SysSetForm::IntToQColor(const int &intColor)
{
    int red = intColor & 255;
    int green = intColor >> 8 & 255;
    int blue = intColor >> 16 & 255;
    return QColor(red, green, blue);
}


void SysSetForm::on_RIAPCb_currentIndexChanged(int index)
{
    DoFilter();
}

void SysSetForm::on_harmCb_currentIndexChanged(int index)
{
    DoFilter();
}

void SysSetForm::on_dataSourComb_currentIndexChanged(int index)
{
    DoFilter();
}

void SysSetForm::on_pushButton_clicked()
{
   DataMgr *pDataMgr = DataMgr::Inst();
   pDataMgr->m_CZTPara.sectSmpCnt = ui->secEdi->text().toInt();
   pDataMgr->m_CZTPara.pixSmcnt = ui->pixEdi->text().toInt();
   pDataMgr->m_CZTPara.aLimit   = ui->limEdi->text().toInt();
   pDataMgr->m_CZTPara.smRate   = ui->samRateEdi->text().toInt();
}
