#ifndef DATAINITDIALOG_H
#define DATAINITDIALOG_H

#include <QDialog>
namespace Ui {
class DataInitDialog;
}

class DataInitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataInitDialog(QWidget *parent = 0);
    DataInitDialog(const QString& t1="",const QString& t2="",const QString& t3="", QWidget *parent = 0);
    ~DataInitDialog();

    void SetTag(const QString& t1, const QString& t2, const QString& t3);
    void SetValue(const QString& t1, const QString& t2, const QString& t3);

private slots:
    void on_pb_cancel_clicked();

    void on_pb_ok_clicked();

public:
    Ui::DataInitDialog *ui;

signals:
    void set_nav(bool statu);
    void ok_click();
    void display_msg(const QString& msg);
    void initialize(const QString& angle, const QString& x, const QString& y);
};

#endif // DATAINITDIALOG_H
