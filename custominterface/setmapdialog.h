#ifndef SCANMAPDIALOG_H
#define SCANMAPDIALOG_H

#include <QDialog>

namespace Ui {
class ScanMapDialog;
}

class SetMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetMapDialog(QWidget *parent = 0);
    ~SetMapDialog();

private:
    Ui::ScanMapDialog *ui;

signals:
    void set_scan_map_name(const QString& msg, int type);
    void display_msg(const QString& msg);
private slots:
    void on_pb_set_map_name_clicked();
};

#endif // SCANMAPDIALOG_H
