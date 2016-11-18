#ifndef MListWidget_H
#define MListWidget_H

#include <QListWidget>

class MListWidget : public QListWidget
{
    Q_OBJECT

public:
    MListWidget(QWidget *parent = 0);
    bool IsCanAdd;
    bool IsCanDel;
    bool CheckIsExist(QString item);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void performDrag();

    QPoint startPos;
};

#endif
