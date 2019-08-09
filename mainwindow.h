#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTreeWidgetItem>
#include<QLineEdit>
#include<QPushButton>
#include<qDebug>
#include<receusb_msg.h>
#include<dealusb_msg.h>
#include<savepcdthread.h>
#include<QTableWidgetItem>
#include<QThread>
#include<QTime>
#include<QTimer>
#include<QMessageBox>
#include<QFileDialog>
#include<filesave.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initGUI();                 //界面的初始化函数
    void initTreeWidget();          //设备寄存器读写的界面的初始化函数
    void initThread();              //要开启的线程的初始化函数
    void initConnect();             //家里信号与槽连接的初始化函数

    QLineEdit *edit_1;
    QPushButton pushButton;

    bool isLinkSuccess;

    //将要开启的线程
    ReceUSB_Msg *recvUsbMsg_obj;
    QThread *recvUsbThread;

    DealUsb_msg *dealUsbMsg_obj;
    QThread *dealUsbThread;

    savePCDThread *savePCD_obj;
    QThread *saveThread;


    QTableWidgetItem tofMinItem_value;
    QTableWidgetItem tofMaxItem_value;
    QTableWidgetItem peakMinItem_value;
    QTableWidgetItem peakMaxItem_value;

    fileSave fileSaveDia;


private:
    Ui::MainWindow *ui;

private slots:
    void pushButton_slot();

    void on_linkUSB_pushButton_clicked();

    void on_readSys_pushButton_clicked();

    void on_writeSys_pushButton_clicked();

    void on_loadSetting_pushButton_clicked();

    void on_saveSetting_pushButton_clicked();

    void on_pushButton_5_clicked();

    void linkInfoSlot(int);         //USB连接信号响应的槽函数

    void reReadSysSlot(QString str);  //读取系统指令 返回槽函数

    void reReadDevSlot(QString str); //读取设备指令 返回槽函数

    void showSaveFileDialog();

    void isSaveFlagSlot(bool,QString,int);

    void statisticsValueSlot(float, float, float,float);

signals:
    void openLinkSignal(int ,int); //打开连接

    void closeLinkSignal();         //关闭连接

    void readSysSignal(int,bool);

    void writeSysSignal(int,QString,bool);

    void readDevSignal(int,int,bool);

    void writeDevSignal(int,int,QString,bool);

    void loadSettingSignal(QString, bool);

    void saveSettingSignal(QString,int, bool );

    void read_usb_signal();     //读取USb信号

};

#endif // MAINWINDOW_H
