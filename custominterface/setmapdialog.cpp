#include "setmapdialog.h"
#include "ui_setmapdialog.h"

SetMapDialog::SetMapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScanMapDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Set Map Name");
}

SetMapDialog::~SetMapDialog()
{
    delete ui;
}

void SetMapDialog::on_pb_set_map_name_clicked()
{
    if(ui->le_set_map_name->text().isEmpty())
    {
        emit display_msg("error:Map name cannot be empty!");
        return;
    }
    emit set_scan_map_name(ui->le_set_map_name->text(), ui->cb_type->currentIndex());
    close();
}
