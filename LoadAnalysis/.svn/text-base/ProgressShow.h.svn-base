#ifndef PROGRESSSHOW_H
#define PROGRESSSHOW_H

#include <QWidget>

namespace Ui {
class ProgressShow;
}

class ProgressShow : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressShow(QWidget *parent = 0);
    ~ProgressShow();

public slots:
    void updateMsg(int val,QString msg);

private:
    Ui::ProgressShow *ui;
};

#endif // PROGRESSSHOW_H
