#include "showsettingdia.h"
#include "ui_showsettingdia.h"
#include<QMessageBox>
#include<QDebug>
#include<QFile>


int ProtocolFlag;      //0: 1*256;  1:2*256    2:4*256

showSettingDia::showSettingDia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showSettingDia)
{
    ui->setupUi(this);

    ui->frameNum_lineEdit->setText("1");
    ui->tofMax_lineEdit->setText("10");
    QString selStr = "1*256";
    ui->comboBox->addItem(selStr);
    selStr = "2*256";
    ui->comboBox->addItem(selStr);
    selStr = "4*256";
    ui->comboBox->addItem(selStr);


    /******读取本地的配置文件*********/
    QFile file("setting.ini");
    QByteArray temp("\r\n");
    QString line[20];

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        int i = 0;
        while (!in.atEnd())
        {
            line[i] = in.readLine();
            i++;
        }
        file.close();
    }
    int numSeri_ = line[0].toInt();
    int baudRateBox_ = line[1].toInt();
    int showFrameNum = line[2].toInt();
    int showAngle = line[3].toInt();
    int maxDistance = line[4].toInt();
    int porocIndex = line[5].toInt();

    ui->frameNum_lineEdit->setText(QString::number(showFrameNum));
    ui->showAngle_lineEdit->setText(QString::number(showAngle));
    ui->tofMax_lineEdit->setText(QString::number(maxDistance));
    ui->comboBox->setCurrentIndex(porocIndex);
    ProtocolFlag = porocIndex;



}

showSettingDia::~showSettingDia()
{
    delete ui;
}

void showSettingDia::on_ok_pushButton_clicked()
{
    int frameNum = ui->frameNum_lineEdit->text().toInt();
    int showAngle = ui->showAngle_lineEdit->text().toInt();
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

    /*******协议选择*******/
    ProtocolFlag = ui->comboBox->currentIndex();
    qDebug()<<"ProtocolFlag = "<<ProtocolFlag<<endl;

    emit showSettingParaSignal(frameNum, showAngle, maxTof);
    this->hide();



    //将配置保存到本地文件
    QFile file("setting.ini");
    QByteArray temp("\r\n");
    QString line[20];

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        int i = 0;
        while (!in.atEnd())
        {
            line[i] = in.readLine();
            i++;
        }
        file.close();
    }


    if(file.open(QIODevice::WriteOnly))
    {
        QByteArray writeData;
        writeData = line[0].toLatin1()+ temp + line[1].toLatin1() + temp+ \
                QString::number(frameNum).toLatin1()+ temp + QString::number(showAngle).toLatin1()+temp+\
                QString::number(maxTof).toLatin1()+ temp +QString::number(ProtocolFlag).toLatin1();
        if (-1 == file.write(writeData))
        {
            qDebug()<<"ERROR";
        }
        file.close();
    }





}

void showSettingDia::on_cancel_pushButton_clicked()
{
    this->hide();
}
