#include "qmylistwidget.h"
#include <QMouseEvent>

QMyListWidget::QMyListWidget(QWidget *parent)
    :QListWidget(parent)
{
    checked_ =false;
    index_ = 0;
}

void QMyListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
    {
        emit right_clicked(currentIndex());
    }
    if(event->button() == Qt::LeftButton)
    {
        if(index_ == currentIndex().row())
        {
            checked_ = !checked_;
        }
        else
        {
            checked_ = true;
            index_ = currentIndex().row();
        }
        emit left_clicked(currentIndex(),checked_);
    }
    //要继续保留QListWidget原有的点击事件.

}
