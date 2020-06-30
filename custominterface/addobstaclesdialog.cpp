#include "addobstaclesdialog.h"
#include "ui_addobstaclesdialog.h"
#include "math.h"

AddObstaclesDialog::AddObstaclesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddObstaclesDialog)
{
    ui->setupUi(this);
}

AddObstaclesDialog::~AddObstaclesDialog()
{
    delete ui;    
}

void AddObstaclesDialog::on_pb_cancel_clicked()
{
    emit set_status(false);
    close();
}

void AddObstaclesDialog::on_pb_redraw_clicked()
{
    emit redraw();
}

void AddObstaclesDialog::on_pb_ok_clicked()
{
    emit save_obstacles();
    emit set_status(false);
    close();
}

void AddObstaclesDialog::show_point(const QString & x, const QString & y)
{
    ui->le_x->setText(x);
    ui->le_y->setText(y);
}

void AddObstaclesDialog::on_pb_save_shape_clicked()
{
    emit save_shape();
}

void AddObstaclesDialog::on_cb_shape_currentIndexChanged(const QString &arg1)
{
    emit set_shape(arg1);
}
