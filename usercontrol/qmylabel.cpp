#include "qmylabel.h"

QMyLabel::QMyLabel(QWidget *parent)
    :QLabel(parent)
{
 //       this->grabKeyboard();
    //    this->grabMouse();
}

void QMyLabel::mousePressEvent(QMouseEvent *ev)
{
    emit click(ev);
}

void QMyLabel::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Left)
    {
        emit key_press(Qt::Key_Left);
    }
    if(ev->key() == Qt::Key_Up)
    {
        emit key_press(Qt::Key_Up);
    }
    if(ev->key() == Qt::Key_Right)
    {
        emit key_press(Qt::Key_Right);
    }
    if(ev->key() == Qt::Key_Down)
    {
        emit key_press(Qt::Key_Down);
    }
}
