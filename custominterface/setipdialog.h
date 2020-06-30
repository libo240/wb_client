#ifndef SETIPDIALOG_H
#define SETIPDIALOG_H

#include <QDialog>

namespace Ui {
class SetIPDialog;
}

class SetIPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetIPDialog(QWidget *parent = 0);
    ~SetIPDialog();

private slots:
    void on_pb_select_accepted();

    void on_pb_select_rejected();

private:
    Ui::SetIPDialog *ui;

signals:
    void display_msg(const QString& msg);
    void set_ip_addr(const QString& control, const QString& message);

};

#endif // SETIPDIALOG_H
