#include "setipdialog.h"
#include "ui_setipdialog.h"

SetIPDialog::SetIPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetIPDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Set IP Address");
}

SetIPDialog::~SetIPDialog()
{
    delete ui;
}

void SetIPDialog::on_pb_select_accepted()
{
    if(ui->le_control->text().isEmpty() || ui->le_message->text().isEmpty())
    {
        emit display_msg("error:IP address cannot be empty!");
        return;
    }
    emit set_ip_addr(ui->le_control->text(), ui->le_message->text());
    close();
}

void SetIPDialog::on_pb_select_rejected()
{
    close();
}
