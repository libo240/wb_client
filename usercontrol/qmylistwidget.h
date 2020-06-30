#ifndef QMYLISTWIDGET_H
#define QMYLISTWIDGET_H

#include <QListWidget>
#include <QAction>
#include <QMenu>
#include <QLabel>

class QMyListWidget:public QListWidget
{
    Q_OBJECT
public:
    QMyListWidget(QWidget *parent = NULL);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void left_clicked(const QModelIndex& index, bool check);
    void right_clicked(const QModelIndex& index);

private:
    bool checked_;
    int index_;

//    QMenu *m_contextMenu;
//    QAction *m_addAction;
//    QAction *m_delAction;
//    QAction *m_loadAction;
//    QLabel* m_label;
};

#endif // QMYLISTWIDGET_H
