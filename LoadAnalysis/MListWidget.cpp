#include <QtGui>

#include "MListWidget.h"
#include <QApplication>

MListWidget::MListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);
    IsCanAdd = true;
    IsCanDel = true;
}

bool MListWidget::CheckIsExist(QString str)
{
    int row = this->count();
    for(int i=0;i<row;i++){
        if(str == this->item(i)->text())
            return true;
    }
    return false;
}

void MListWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QListWidget::mousePressEvent(event);
}

void MListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QListWidget::mouseMoveEvent(event);
}

void MListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    MListWidget *source =
            qobject_cast<MListWidget *>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    MListWidget *source =
            qobject_cast<MListWidget *>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MListWidget::dropEvent(QDropEvent *event)
{
    MListWidget *source =
            qobject_cast<MListWidget *>(event->source());
    if (source && source != this) {
        if(IsCanAdd){
            if(!CheckIsExist(event->mimeData()->text())){
           // addItem(event->mimeData()->text());
            }
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MListWidget::performDrag()
{
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/images/person.png"));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
            if(IsCanDel){
                delete item;
            }

    }
}
