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
#include<showsettingdia.h>
#include<QSignalMapper>
#include<QLabel>
#include<statisticsdialog.h>
#include<qcustomplot.h>
#include<calmeanstdthread.h>



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
    void initConnect();             //信号与槽连接的初始化函数
    void initSerial();              //初始化串口的函数

    QLineEdit TDC_lineEdit[13];
    QPushButton TDC_read_pushButton[13];
    QPushButton TDC_write_pushButton[13];
    QSignalMapper *TDC_read_signalMapper;
    QSignalMapper *TDC_write_signalMapper;

    QLineEdit Integration_lineEdit[4];
    QPushButton Integration_read_pushButton[4];
    QPushButton Integration_write_pushButton[4];
    QSignalMapper *Integration_read_signalMapper;
    QSignalMapper *Integration_write_signalMapper;

    QLineEdit MA_lineEdit[16];
    QPushButton MA_read_pushButton[16];
    QPushButton MA_write_pushButton[16];
    QSignalMapper *MA_read_signalMapper;
    QSignalMapper *MA_write_signalMapper;

    QLineEdit Digital_lineEdit[10];
    QPushButton Digital_read_pushButton[10];
    QPushButton Digital_write_pushButton[10];
    QSignalMapper *Digital_read_signalMapper;
    QSignalMapper *Digital_write_signalMapper;

    QLineEdit Analog_lineEdit[26];
    QPushButton Analog_read_pushButton[26];
    QPushButton Analog_write_pushButton[26];
    QSignalMapper *Analog_read_signalMapper;
    QSignalMapper *Analog_write_signalMapper;

    QLineEdit Pixel_lineEdit[17];
    QPushButton Pixel_read_pushButton[17];
    QPushButton Pixel_write_pushButton[17];
    QSignalMapper *Pixel_read_signalMapper;
    QSignalMapper *Pixel_write_signalMapper;

    QLineEdit Top_lineEdit[13];
    QPushButton Top_read_pushButton[13];
    QPushButton Top_write_pushButton[13];
    QSignalMapper *Top_read_signalMapper;
    QSignalMapper *Top_write_signalMapper;

    QLineEdit Delayline_lineEdit[7];
    QPushButton Delayline_read_pushButton[7];
    QPushButton Delayline_write_pushButton[7];
    QSignalMapper *Delayline_read_signalMapper;
    QSignalMapper *Delayline_write_signalMapper;

    QLineEdit MISC_lineEdit[8];
    QPushButton MISC_read_pushButton[8];
    QPushButton MISC_write_pushButton[8];
    QSignalMapper *MISC_read_signalMapper;
    QSignalMapper *MISC_write_signalMapper;

    QLineEdit Others_lineEdit[6];
    QPushButton Others_read_pushButton[6];
    QPushButton Others_write_pushButton[6];
    QSignalMapper *Others_read_signalMapper;
    QSignalMapper *Others_write_signalMapper;


    bool isLinkSuccess;

    //将要开启的线程
    ReceUSB_Msg *recvUsbMsg_obj;
    QThread *recvUsbThread;

    DealUsb_msg *dealUsbMsg_obj;
    QThread *dealUsbThread;

    savePCDThread *savePCD_obj;
    QThread *saveThread;

    calMeanStdThread * calMean_obj;
    QThread *calMeanThread;


    QTableWidgetItem tofMinItem_value;
    QTableWidgetItem tofMaxItem_value;
    QTableWidgetItem peakMinItem_value;
    QTableWidgetItem peakMaxItem_value;
    float tofMin_, tofMax_, peakMin_, peakMax_;

    QTimer oneSecondTimer;   //1sec show

    fileSave fileSaveDia;

    showSettingDia showSettingDia_;
    statisticsDialog  statisticsDia_;


    QLabel explainLabel;  //显示注释
    int expandItem_index;

    int framePerSecond;    //显示帧率


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

    void reReadDevSlot(int regesiterAddress,QString str); //读取设备指令 返回槽函数

    void showSaveFileDialog();

    void isSaveFlagSlot(bool,QString,int);

    void statisticsValueSlot(float, float, float,float);

    void oneSecondTimer_slot();

    void showShowSettingDialog();

    void showSettingParaSlot(int FrameNum, int Angle ,int TOFmax);

    void showRunInfoSlot(QString msgStr);     //显示运行日志的槽函数



    /*******************************************/
    void TDC_read_slot(int);

    void TDC_write_slot(int);

    void Integration_read_slot(int);

    void Integration_write_slot(int);

    void MA_read_slot(int);

    void MA_write_slot(int);

    void Digital_read_slot(int);

    void Digital_write_slot(int);

    void Analog_read_slot(int);

    void Analog_write_slot(int);

    void Pixel_read_slot(int);

    void Pixel_write_slot(int);

    void Top_read_slot(int);

    void Top_write_slot(int);

    void Delayline_read_slot(int);

    void Delayline_write_slot(int);

    void MISC_read_slot(int);

    void MISC_write_slot(int);

    void Others_read_slot(int);

    void Others_write_slot(int);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemExpanded(QTreeWidgetItem *item);

    void showStatisticDia_slot();

    void on_getALL_pushButton_clicked();

    void on_setAll_pushButton_clicked();

    void on_toolBox_currentChanged(int index);

    void on_changeTofPeak_pushButton_clicked();

    void on_openSerial_pushButton_clicked();

    void on_serialPlay_pushButton_clicked();

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

    void changeTofPeak_signal();

    void openSerial_signal(Settings,bool);

};

#endif // MAINWINDOW_H
