#include "datainitdialog.h"
#include "ui_datainitdialog.h"

DataInitDialog::DataInitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataInitDialog)
{
    ui->setupUi(this);
}

DataInitDialog::DataInitDialog(const QString& t1,const QString& t2,const QString& t3, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DataInitDialog)
{
    ui->setupUi(this);
    ui->le_angle->setText(t1);
    ui->le_x->setText(t2);
    ui->le_y->setText(t3);
}

DataInitDialog::~DataInitDialog()
{
    delete ui;
}

void DataInitDialog::SetTag(const QString &t1, const QString &t2, const QString &t3)
{
    ui->label->setText(t1);
    ui->label_2->setText(t2);
    ui->label_3->setText(t3);
}

void DataInitDialog::SetValue(const QString &t1, const QString &t2, const QString &t3)
{
    ui->le_angle->setText(t1);
    ui->le_x->setText(t2);
    ui->le_y->setText(t3);
}

void DataInitDialog::on_pb_cancel_clicked()
{
    emit set_nav(false);
    deleteLater();
}

void DataInitDialog::on_pb_ok_clicked()
{
    if(ui->le_angle->text().isEmpty() || ui->le_x->text().isEmpty() || ui->le_y->text().isEmpty())
    {
        emit display_msg("error:angle pos_x pos_y can not be empty!");
        return;
    }

    emit ok_click();
    emit initialize(ui->le_angle->text(), ui->le_x->text(), ui->le_y->text());
    emit set_nav(false);
    deleteLater();
}
