#ifndef QMYLABLE_H
#define QMYLABLE_H
#include <QLabel>
#include <QKeyEvent>

class QMyLabel : public QLabel
{
    Q_OBJECT
public:
    QMyLabel(QWidget *parent=Q_NULLPTR);

protected:
    virtual void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent* ev);

signals:
    void click(QMouseEvent *ev);
    void key_press(Qt::Key key);

};

#endif // QMYLABLE_H
