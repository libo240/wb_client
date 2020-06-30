#ifndef SYNCMAPDIALOG_H
#define SYNCMAPDIALOG_H

#include <QDialog>

namespace Ui {
class SyncMapDialog;
}

class SyncMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SyncMapDialog(QString control_type, QWidget *parent = 0);
    ~SyncMapDialog();
public:
    void UpdateProgess(int progress);

private slots:

    void on_pb_cancle_clicked();

    void on_pb_ok_clicked();

    void on_pb_cancle_2_clicked();

    void on_pb_ok_2_clicked();

signals:
    void sync_map(const QString& conrtol_type_, const QString& data_type_);
    void display_msg(const QString& msg);

private:
    Ui::SyncMapDialog *ui;
    QString control_type_;
    QString data_type_;
};

#endif // SYNCMAPDIALOG_H
