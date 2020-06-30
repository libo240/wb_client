#ifndef ADDOBSTACLESDIALOG_H
#define ADDOBSTACLESDIALOG_H

#include <QDialog>
#include <jsoncpp/json/json.h>

namespace Ui {
class AddObstaclesDialog;
}

class AddObstaclesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddObstaclesDialog(QWidget *parent = 0);
    ~AddObstaclesDialog();

private slots:
    void on_pb_cancel_clicked();
    void on_pb_redraw_clicked();
    void on_pb_ok_clicked();
    void on_pb_save_shape_clicked();

    void on_cb_shape_currentIndexChanged(const QString &arg1);

private:
    Ui::AddObstaclesDialog *ui;

public slots:
    void show_point(const QString &x, const QString &y);
signals:
    void display_msg(const QString& msg);
    void redraw();
    void set_shape(const QString &obstacles);
    void save_shape();
    void save_obstacles();
    void set_status(bool set_status);
};

#endif // ADDOBSTACLESDIALOG_H
