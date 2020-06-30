#include "syncmapdialog.h"
#include "ui_syncmapdialog.h"
//#include "QDialogButtonBox"
#include "QPushButton"

SyncMapDialog::SyncMapDialog(QString control_type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyncMapDialog),
    control_type_(control_type)
{
    ui->setupUi(this);
    this->setWindowTitle("Sync Map");
    ui->bar_progress->setVisible(false);
    ui->le_progress->setVisible(false);
    ui->pb_cancle_2->setVisible(false);
    ui->pb_ok_2->setVisible(false);
}

SyncMapDialog::~SyncMapDialog()
{
    delete ui;
}

void SyncMapDialog::UpdateProgess(int progress)
{
    if(0 <= progress && 100 >= progress)
    {
        ui->bar_progress->setValue(progress);
    }
    if(progress >= 100)
    {
        ui->pb_ok_2->setEnabled(true);
    }
}

void SyncMapDialog::on_pb_cancle_clicked()
{
    close();
}

void SyncMapDialog::on_pb_ok_clicked()
{
    data_type_ = "";
    if(ui->cb_vector->isChecked())
    {
        data_type_ += "vector,";
    }
    if(ui->cb_map_2d->isChecked())
    {
        data_type_ += "map_2d,";
    }
    if(ui->cb_map_tile->isChecked())
    {
        data_type_ += "map_tile,";
    }
    if(ui->cb_path_net->isChecked())
    {
        data_type_ += "path_net,";
    }
    if(data_type_.endsWith(','))
    {
        data_type_ = data_type_.mid(0,data_type_.size()-1);
    }
    if(data_type_.isEmpty())
    {
        emit display_msg("error:At least one data type must be selected!");
        return;
    }

    ui->cb_vector->setVisible(false);
    ui->cb_map_2d->setVisible(false);
    ui->cb_map_tile->setVisible(false);
    ui->cb_path_net->setVisible(false);
    ui->le_data_type->setVisible(false);
    ui->pb_cancle->setVisible(false);
    ui->pb_ok->setVisible(false);

    ui->bar_progress->setVisible(true);
    ui->le_progress->setVisible(true);
    ui->pb_cancle_2->setVisible(true);
    ui->pb_ok_2->setVisible(true);

    emit sync_map(control_type_, data_type_);
}

void SyncMapDialog::on_pb_cancle_2_clicked()
{
    emit sync_map("cancle",data_type_);
    close();
}

void SyncMapDialog::on_pb_ok_2_clicked()
{
    close();
}
