#include "showsettingdia.h"
#include "ui_showsettingdia.h"
#include<QMessageBox>

showSettingDia::showSettingDia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showSettingDia)
{
    ui->setupUi(this);

    ui->frameNum_lineEdit->setText("1");
    ui->tofMax_lineEdit->setText("10");
}

showSettingDia::~showSettingDia()
{
    delete ui;
}

void showSettingDia::on_ok_pushButton_clicked()
{
    int frameNum = ui->frameNum_lineEdit->text().toInt();
    int maxTof = ui->tofMax_lineEdit->text().toInt();
    if(frameNum<1)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("输入值非法"));
        ui->frameNum_lineEdit->setText("1");
        frameNum = 1;
    }

    if(maxTof<1)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("输入值非法"));
        ui->tofMax_lineEdit->setText("10");
        maxTof = 10;
    }

    emit showSettingParaSignal(frameNum,maxTof);
    this->hide();

}

void showSettingDia::on_cancel_pushButton_clicked()
{
    this->hide();
}
