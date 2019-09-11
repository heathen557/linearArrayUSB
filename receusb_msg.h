#ifndef RECEUSB_MSG_H
#define RECEUSB_MSG_H

#include <QObject>
#include<QDebug>
#include<QTimer>
#include<Windows.h>
#include"lusb0_usb.h"
#include<QFile>
#include<QtSerialPort/QSerialPort>
#include<QMessageBox>


struct Settings {
    QString name;
    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;
};

class ReceUSB_Msg : public QObject
{
    Q_OBJECT
public:
    explicit ReceUSB_Msg(QObject *parent = 0);

    struct usb_device *findUSBDev(const unsigned short idVendor,
                                  const unsigned short idProduct);

    bool openUSB(struct usb_device *dev);


    bool System_Register_Read(int Address, QString &Data);

    bool System_Register_Write(int Address, QString &Data);

    bool Device_Register_Read(int slavedId,int Address,QString &Data);

    bool Device_Register_Write(int slavedId,int Address,QString &Data);


    bool devOpenFlg;

    usb_dev_handle *devHandle;

    struct usb_device * dev;

    int lastSpadNum;


    bool isFirstLink;   //因为USB驱动存在问题，故设次函数

    int idVendor_,idProduct_;


     QByteArray tmpArray;    //用于协议2的缓存 缓存到260个字节时发送给数据处理线程
    /*********串口相关******************/
    QSerialPort *serial;
    QString m_buffer;   //接收数据的缓存区



signals:
    void linkInfoSignal(int );      //向主线程发送链接信息（错误警告）
                                    // 0：连接正常 1没找到设备
                                    // 2:没有接收到数据  3打开设备失败
                                    // 4：读取系统成功；5：读取系统失败；
                                    // 6：读取设备成功；7：读取设备失败
                                    // 8：加载配置信息成功；9：加载配置信息失败
                                    // 10：保存配置信息成功； 11：保存配置信息失败
                                    // 12：写入系统成功      13：写入系统失败
                                    // 14：写入设备成功      15：写入设备失败
                                    // 16:串口打开成功       17：串口打开失败
    void staticValueSignal(float,float,float,float,float,float,float,float,float,float);

    void recvMsgSignal(QByteArray); //发送给处理线程的信号

    void recvMsgSignal_2_256(QByteArray); //发送给处理线程的信号 2*256的协议

    void recvMsgSignal_4_256(QByteArray); //发送给处理线程的信号

    void reReadSysSignal(QString);  //读取系统指令 返回信号

    void reReadDevSignal(int,QString); //读取设备指令 返回信号




public slots:
    void read_usb();                 //读取USB内容的槽函数
    void openLinkDevSlot();          //打开设备连接的槽函数
    void run(int vId, int pId);
    void closeUSB();

    void readSysSlot(int address,bool recvFlag);
    void writeSysSlot(int addr,QString data,bool recvFlag);
    void readDevSlot(int id,int address,bool recvFlag);
    void writeDevSlot(int slavId,int addr,QString data,bool recvFlag);
    void loadSettingSlot(QString filePath,bool recvFlag);
    void saveSettingSlot(QString filePath,int deviceId,bool  recvFlag);


    //串口相关
    void openSerial_slot(Settings serialSetting, bool openFlag);
    void recvSerial_slot();
    void singleDataDeal(QString singleData);
    QByteArray stringToByte(QString str);
    bool msgCheck(QString strMsg);

};

#endif // RECEUSB_MSG_H
