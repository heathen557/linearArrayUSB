﻿
#include "receusb_msg.h"
#include<QDataStream>


#define MY_CONFIG 1
#define MY_INTF 0



//extern bool isShowPointCloud;
//extern bool isWriteSuccess;    //写入命令是否成功标识
extern bool isRecvFlag;
//extern int framePerSecond;

extern int ProtocolFlag;  //0: 1*256;  1:2*256    2:4*256




ReceUSB_Msg::ReceUSB_Msg(QObject *parent) : QObject(parent)
{
    qDebug()<<"ReceUSB_Msg thread start!!!!"<<endl;

    isFirstLink = true;
    isRecvFlag = false;

    serial = NULL;

}

//查找是不是存在想要链接的USB设备
struct usb_device *ReceUSB_Msg::findUSBDev(const unsigned short idVendor,
                                           const unsigned short idProduct)
{
    struct usb_bus *bus;
    struct usb_device *dev;

//    usb_find_busses(); /* find all busses */
//    usb_find_devices(); /* find all connected devices */

    for(bus = usb_get_busses(); bus; bus = bus->next)
    {
        for(dev = bus->devices; dev; dev = dev->next)
        {
            if((dev->descriptor.idVendor == idVendor_)
                    && (dev->descriptor.idProduct == idProduct_))
            {
                return dev;
            }else
            {
                emit linkInfoSignal(1);  //没找到设备
            }
        }
    }
    return NULL;

}

//打开已经存在的USB设备
bool ReceUSB_Msg::openUSB(struct usb_device *dev)
{
    devOpenFlg = false;

    devHandle = usb_open(dev);
    if(!devHandle)
    {
        qDebug() << "error opening device: ";
        qDebug() << usb_strerror();
        return false;
    }
    else
    {
        qDebug() << "success: device " << "MY_VID" << " : "<<  "MY_PID" << " opened";
        devOpenFlg = true;
    }

    if (usb_set_configuration(devHandle, MY_CONFIG) < 0)
    {
        qDebug() << "error setting config #" << MY_CONFIG << " : " << usb_strerror();
        usb_close(devHandle);
        return false;
    }
    else
    {
        qDebug() << "success: set configuration #" << MY_CONFIG;
    }

    if (usb_claim_interface(devHandle, MY_INTF) < 0)
    {
        qDebug() << "error claiming interface #" << MY_INTF;
        qDebug() << usb_strerror();
        usb_close(devHandle);
        return false;
    }
    else
    {
        qDebug() <<  "success: claim_interface #" << MY_INTF;
    }

    return true;
}


//关闭USB链接
void ReceUSB_Msg::closeUSB()
{
    if(devHandle)
    {
        devOpenFlg = false;
        //        UsbListener::quit();
        int ret = usb_close(devHandle); // Exit Thread
        qDebug() << "Close USB Device [" << ret << "]";
        devHandle = NULL;
        isFirstLink = false;   //以后就不是第一次连接了

        emit showRunInfoSignal("USB link closed");
    }

}



//系统FPGA注册 读取
bool ReceUSB_Msg::System_Register_Read(int Address, QString &Data)
{

    bool res = true;
    int transLen = 0;
    struct usb_ctrl_setup Cmd;
    Cmd.bRequestType = 0xC0;
    Cmd.bRequest = 0x01;
    Cmd.wValue = 0x0000;
    Cmd.wIndex = Address;  //need change
    Cmd.wLength = 0x0001;
    char data[1];
    QString arr;
    int num = usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);
//    Data = QString(data[0]);

    quint8 tmp = quint8(data[0]);
    Data = QString::number(tmp);

    if(num>0)
    {
        res= true;
        QString str = "System_Register_Read [R]addr="+ QString::number(Address,16).toUpper() + ", msg="+QString::number(tmp,16).toUpper()+" ,res=1 ";
        emit showRunInfoSignal(str);

    }else
    {
        res = false;
        QString str = "System_Register_Read [R]addr="+ QString::number(Address,16).toUpper() + ", msg="+QString::number(tmp,16).toUpper()+" ,res=0 ";
        emit showRunInfoSignal(str);
    }

    return res;
}


//系统FPGA注册 写入
bool ReceUSB_Msg::System_Register_Write(int Address, QString &Data)
{
    bool res = true;
    int transLen = 0;
    char data[1];
    struct usb_ctrl_setup Cmd;
    Cmd.bRequestType = 0x40;
    Cmd.bRequest = 0x01;
    Cmd.wValue = 0x0000;
    Cmd.wIndex = Address;  // need change
    Cmd.wLength = 0x0001;
//    data[0] = Data.toInt(NULL,16);    //need modify
    data[0] = quint8(Data.toInt(NULL,16));

    int num = usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);

    if(num>0)
    {
        res = true;

        QString str = "System_Register_Write [W]addr="+ QString::number(Address,16).toUpper() + ", msg="+Data+" ,res=1 ";
        emit showRunInfoSignal(str);

    }else
    {
        res = false;
        QString str = "System_Register_Write [W]addr="+ QString::number(Address,16).toUpper() + ", msg="+Data+" ,res=0 ";
        emit showRunInfoSignal(str);
    }

    return res;
}

//设备注册 读取
bool ReceUSB_Msg::Device_Register_Read(int slavedId,int Address,QString &Data)
{
    bool res = true;
    int transLen = 0;
    char data[1];
    struct usb_ctrl_setup Cmd;
    Cmd.bRequestType = 0x40;
    Cmd.bRequest = 0x01;
    Cmd.wValue = 0x0000;
    Cmd.wIndex = 0x00f1;
    Cmd.wLength = 0x0001;
    data[0]= slavedId;   //need modify
    int num;
    num = usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);


    Cmd.wIndex = 0x00f5;
    data[0] = 0x33;
    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);


    Cmd.wIndex = 0x00f2;
    data[0] = Address;      //need modify
    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);


    Cmd.wIndex = 0x00f5;
    data[0] = 0xf9;
    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);


    Cmd.bRequestType = 0xC0;
    Cmd.wIndex = 0x00f4;
    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,data,1,transLen);

    quint8 tmp = quint8(data[0]);
    Data = QString::number(tmp);

    if(num>0)
    {
        res = true;
        QString str = "Device_Register_Read [R]addr="+ QString::number(Address,16).toUpper() + ", msg="+QString::number(tmp,16).toUpper()+" ,res=1 ";
        emit showRunInfoSignal(str);
    }else
    {
        res = false;
        QString str = "Device_Register_Read [R]addr="+ QString::number(Address,16).toUpper() + ", msg="+QString::number(tmp,16).toUpper()+" ,res=0 ";
        emit showRunInfoSignal(str);
    }

    //此处返回的是  十进制数字
    return res;
}


//设备注册 写入
bool ReceUSB_Msg::Device_Register_Write(int slavedId,int Address,QString &Data)
{
    bool res = true;
    int transLen = 0;
    uchar data[1];
    struct usb_ctrl_setup Cmd;
    Cmd.bRequestType = 0x40;
    Cmd.bRequest = 0x01;
    Cmd.wValue = 0x0000;
    Cmd.wIndex = 0x00f1;
    Cmd.wLength = 0x0001;
    data[0] = slavedId;      //need modify

    int num;
    num = usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,(char*)(&data[0]),1,transLen);

    Cmd.wIndex = 0x00f5;
    data[0] = 0x37;
    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,(char*)(&data[0]),1,transLen);


    Cmd.wIndex = 0x00f2;
    data[0] = Address;      //need modify
    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,(char*)(&data[0]),1,transLen);


    Cmd.wIndex = 0x00f3;

    int tmp = Data.toInt(NULL,16);
    data[0] = tmp;

    num += usb_control_msg(devHandle,Cmd.bRequestType,Cmd.bRequest,Cmd.wValue,Cmd.wIndex,(char*)(&data[0]),1,transLen);

    if(num>0)
    {
        res = true;
        QString str = "Device_Register_Write [W]addr="+ QString::number(Address,16).toUpper() + ", msg="+Data.toUpper() +" ,res=1 ";
        emit showRunInfoSignal(str);
    }else
    {
        res = false;
        QString str = "Device_Register_Write [W]addr="+ QString::number(Address,16).toUpper() + ", msg="+Data.toUpper() +" ,res=0 ";
        emit showRunInfoSignal(str);
    }


    return res;
}




//读取USB设备数据
void ReceUSB_Msg::read_usb()
{
    int ret;
    char MyBuffer[4096];
    //批量读(同步)
    QByteArray mArray;

    while(isRecvFlag)
    {
        ret = usb_interrupt_read(devHandle,129,MyBuffer,sizeof(MyBuffer),3000);
//            ret = usb_bulk_read(devHandle, 129, MyBuffer, sizeof(MyBuffer), 3000);       //此处延迟设置为3000，经过测试设置为1的时候，ret<0,程序报错退出

        if (ret < 0) {
            qDebug("**************************************************error reading:%s", usb_strerror());
            emit linkInfoSignal(2);  //  2:没有接收到数据
            emit showRunInfoSignal(QStringLiteral("device can not receive data"));
            break;
        }

        if(260 == ret)
        {
            mArray = QByteArray(MyBuffer,260);

            if(0 == ProtocolFlag )
                emit recvMsgSignal(mArray);
            if(1 == ProtocolFlag)
                emit recvMsgSignal_2_256(mArray);
            if(2 == ProtocolFlag)
                emit recvMsgSignal_4_256(mArray);

//            QDataStream out(&mArray,QIODevice::ReadWrite);
//            QString strHex;
//            while (!out.atEnd())
//            {
//                qint8 outChar=0;
//                out>>outChar;
//                QString str=QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));

//                if (str.length()>1)
//                {
//                    strHex+=str+" ";
//                }
//                else
//                {
//                    strHex+="0"+str+" ";
//                }
//            }
//            strHex = strHex.toUpper();

//            qDebug()<<QStringLiteral("原始数据为：")<<strHex<<endl;
        }else if(ret<260)
        {
            mArray = QByteArray(MyBuffer,ret);

//            if(4 == ret && 0 == tmpArray.size())     //2019-11-01
//            {
//                tmpArray.append(mArray);
//            }

            if(4 == ret)
            {
                tmpArray.clear();
                tmpArray.append(mArray);
            }
            if(256 == ret && 4 == tmpArray.size())
            {
                tmpArray.append(mArray);
//                qDebug()<<"tmpArray's size = "<<tmpArray.size()<<" ProtocolFlag="<<ProtocolFlag<<endl;

                if(0 == ProtocolFlag )
                    emit recvMsgSignal(tmpArray);
                if(1 == ProtocolFlag )
                    emit recvMsgSignal_2_256(tmpArray);
                if(2 == ProtocolFlag )
                    emit recvMsgSignal_4_256(tmpArray);

//                QDataStream out(&tmpArray,QIODevice::ReadWrite);
//                QString strHex;
//                while (!out.atEnd())
//                {
//                    qint8 outChar=0;
//                    out>>outChar;
//                    QString str=QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));

//                    if (str.length()>1)
//                    {
//                        strHex+=str+" ";
//                    }
//                    else
//                    {
//                        strHex+="0"+str+" ";
//                    }
//                }
//                strHex = strHex.toUpper();

//                qDebug()<<QStringLiteral("原始数据为：")<<strHex<<endl;




                tmpArray.clear();
            }
        }

    }


}

//接收主函数信号，开启连接
void ReceUSB_Msg::run(int vId, int pId)
{
    idVendor_ = vId;
    idProduct_ = pId;

    openLinkDevSlot();

}

//打开设备连接
void ReceUSB_Msg::openLinkDevSlot()
{
    usb_init();                                  /* initialize the library */


    //下面代码屏蔽，重新打开时会查询不到设备
    int numBus = usb_find_busses();              /* find all busses */
    qDebug()<<"thread NUMbUS = "<<numBus<<endl;

    int numDevs = usb_find_devices();           /* find all connected devices */
    qDebug()<<"numDevs = "<<numDevs<<endl;

    if(isFirstLink)
    {
        if(numBus <= 0 || numDevs<=0)
        {
            emit linkInfoSignal(1);                //没有发现设备
             emit showRunInfoSignal(QStringLiteral("can not find USB devices"));
            return;
        }
    }


    dev = findUSBDev(0,0);                      //查找usb设备

    if(NULL == dev)
    {
        isFirstLink = false;
        return ;
    }

    if(true ==openUSB(dev))                    //打开USB设备
    {
        emit linkInfoSignal(0);                //打开设备成功
        emit showRunInfoSignal(QStringLiteral("open USB device success "));
    }else{
        emit linkInfoSignal(3);                //打开设备失败
        emit showRunInfoSignal(QStringLiteral("can not open USB device"));
    }

}


//读取系统寄存器槽函数
void ReceUSB_Msg::readSysSlot(int address,bool recvFlag)
{
    QString array;
    //系统注册 读取  0x13 = 19,array返回值
    //    bool res = System_Register_Read(19,array);

    bool res = System_Register_Read(address,array);
    qDebug()<<"[R]sys Read array="<<array<<"   res="<<res<<endl;
    //系统注册 写入测试

    if(res)
    {
        emit reReadSysSignal(array);
        emit linkInfoSignal(4);
    }else
    {
        emit linkInfoSignal(5);
    }

    isRecvFlag = recvFlag;
    read_usb();

}

//写入系统寄存器槽函数
// 12：写入系统成功      13：写入系统失败
void ReceUSB_Msg::writeSysSlot(int addr,QString data,bool recvFlag)
{
    if(System_Register_Write(addr, data))
    {
        emit linkInfoSignal(12);
    }
    else
    {
        emit linkInfoSignal(13);
    }

    isRecvFlag = recvFlag;
    read_usb();
}

//读取设备寄存器槽函数
void ReceUSB_Msg::readDevSlot(int id,int address,bool recvFlag)
{
    QString data;

    if(true == Device_Register_Read(id, address, data))
    {
        emit reReadDevSignal(address,data);
        emit linkInfoSignal(6);
    }else
    {
        emit linkInfoSignal(7);
    }

    isRecvFlag = recvFlag;
    read_usb();


}

//写入设备寄存器槽函数
void ReceUSB_Msg::writeDevSlot(int slavId,int addr,QString data,bool recvFlag)
{
    if(true == Device_Register_Write(slavId,addr,data))
    {
        emit linkInfoSignal(14);
    }else
    {
        emit linkInfoSignal(15);
    }

    isRecvFlag = recvFlag;
    read_usb();

}

//加载配置集槽函数
void ReceUSB_Msg::loadSettingSlot(QString filePath,bool recvFlag)
{
    QFile file(filePath);
    QString line[20];

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        int i = 0;
        while (!in.atEnd())
        {
            line[i] = in.readLine();
            qDebug()<<"line["<<i<<"] = "<<line[i];
            i++;
        }
        file.close();
    }
    //line[0] 对应 str1;
    //line[1] 对应 str2;
    //line[2] 对应 str3;

    bool res = true;
    QString array;
    /**********************测试STR2******************************************/
    // 1、 0x11= 17  0x41=65    （17-22）（0x11 0x16）
    //    QString str2 = "41 01 00";
    QString str2 = line[1];
//    res = System_Register_Write(17,str2.mid(0,2));
//    qDebug()<<"[w]sys write str2="<<str2.mid(0,2)<<"   res="<<res<<endl;

//    // 0x12 = 18
//    res = System_Register_Write(18,str2.mid(3,2));
//    qDebug()<<"[w]sys write str2="<<str2.mid(3,2)<<"   res="<<res<<endl;

//    //0xe2 = 226
//    res = System_Register_Write(226,str2.mid(6,2));
//    qDebug()<<"[w]sys write str2="<<str2.mid(6,2)<<"   res="<<res<<endl;

//    //0x13 = 19
//    res = System_Register_Read(19,array);
//    qDebug()<<"[R]sys Read array="<<array<<"   res="<<res<<endl;

    for(int n=0; n<6; n++)
    {
        res = System_Register_Write(17+n,str2.mid(n*3,2));
        qDebug()<<"[R]sys Read array="<<array<<"   res="<<res<<endl;
    }





    /************************测试STR3********************************************/
    //    QString str3 = "00 00 26 0A 00 64 00 14 00 01 00 00";   //len = 12    (0x20-0x2c)
    QString str3 = line[2];

    //起始位置从32 开始
    for(int i=0 ; i<12; i++)
    {
        res = System_Register_Write(32+i, str3.mid(i*3,2));
        qDebug()<<"[w]sys write str3="<<str3.mid(i*3,2)<<"   res="<<res<<endl;
    }


    /*************************测试STR1******************************************************/
    //    QString str1 = "00 44 1F 44 45 44 EE 02 64 11 22 44 88 88 44 22 11 03 40 00 1F E0 81 4A 84 08 00 00 CC 01 00 00 00 00 00 00 00 0A 06 06 06 06 06 34 FF FF FF FF 04 1E";
    QString str1 = line[0];

    QString str = "34";               //0x2b = 43                        //2019-8-15
    res = Device_Register_Write(216,43,str);
    qDebug()<<"[w]Device write str1="<<"00"<<"   res="<<res<<endl;

    str = "4A";              //0x17 =23                         //2019-8-15
    res = Device_Register_Write(216,23,str);
    qDebug()<<"[w]Device write str1="<<"00"<<"   res="<<res<<endl;




    //0xd8 = 216,
    for(int k=0; k<50; k++)
    {
        res = Device_Register_Write(216,k,str1.mid(3*k,2));
        qDebug()<<"[w]Device write str1="<<str1.mid(3*k,2)<<"   res="<<res<<str1.mid(3*k,2).toInt(NULL,16) <<endl;
    }

        //0号寄存器写1 -》48号寄存器写44 -》1ms ->48号寄存器写04 -》0号寄存器写0     //2019-11-05
      int dataNum = 1;   //此命令只指定最低位，故不需要做位操作    写入1
      QString writeData = QString::number(dataNum,16);
      Device_Register_Write(216,0,writeData);

      //48号寄存器写44
     dataNum = 68;
     writeData = QString::number(dataNum,16);
     Device_Register_Write(216,48,writeData);

     Sleep(1);

      //48号寄存器写04
     dataNum = 4;
     writeData = QString::number(dataNum,16);
     Device_Register_Write(216,48,writeData);


      dataNum = 0;   //此命令只指定最低位，故不需要做位操作    写入1
      writeData = QString::number(dataNum,16);
      Device_Register_Write(216,0,writeData);


    //0x15 = 21
    str = "01";
    res = System_Register_Write(21, str);
    qDebug()<<"[w]sys write str="<<"01"<<"   res="<<res<<endl;


    if(res)
    {
        emit linkInfoSignal(8);
        emit showRunInfoSignal(QStringLiteral("load setting file success"));
    }else
    {
        emit showRunInfoSignal(QStringLiteral("load setting file fail !"));
        emit linkInfoSignal(9);
    }

    /************************开始接受数据****************************************************/
    //    readTimer->start(1);
//    isRecvFlag =true;
//    read_usb();
}

//保存配置集槽函数
// deviceId转换过来的是十进制数  D8->216
void ReceUSB_Msg::saveSettingSlot(QString filePath,int deviceId,bool recvFlag)
{
//    qDebug()<<" setting fileNamePath = "<< filePath<<"  deviceId ="<<deviceId<<endl;

//    int i=0;
//    bool res;
//    QString dataStr;
//    QString textString;

//    QByteArray ba;
////    const char *c_str;
//    int m;
//    /***************文本中写入str1******************************************/
//    for(; i<50 ; i++)
//    {
//        res = Device_Register_Read(deviceId, i, dataStr);
//        m = dataStr.toInt();
//        QString tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//        textString.append(tmpData);
//        qDebug()<<" the data =  "<<m<<"    sixteen's num ="<< tmpData<<endl;

//    }
//    QFile file(filePath);
//    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
//    QTextStream out(&file);
//    out<<textString.toLocal8Bit()<<endl;


//    /***************文本中写入str2*****************************************************/
//    res = System_Register_Read(17, dataStr);
//    m = dataStr.toInt();
//    QString tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//    textString = tmpData;

//    res = System_Register_Read(18, dataStr);
//    m = dataStr.toInt();
//    tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//    textString.append(tmpData);


//    res = System_Register_Read(226, dataStr);
//    m = dataStr.toInt();
//    tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//    textString.append(tmpData);

//    out<<textString.toLocal8Bit()<<endl;


//    /***************文本中写入str3*****************************************************/
//    textString.clear();
//    for (i = 0; i < 12; i++)
//    {
//        res = System_Register_Read((32+i), dataStr);
//        m = dataStr.toInt();
//        tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//        textString.append(tmpData);
//    }
//    out<<textString.toLocal8Bit()<<endl;



//    file.close();
//    if(recvFlag)
//    {
//        isRecvFlag = true;
//        qDebug()<<"saveSettingSlot has set the recvFlag =  true"<<endl;
//        read_usb();
//    }

//    if(res)
//    {
//        linkInfoSignal(10);
//    }else
//    {
//        linkInfoSignal(11);
//    }

    qDebug()<<" setting fileNamePath = "<< filePath<<"  deviceId ="<<deviceId<<endl;

    int i=0;
    bool res;
    QString dataStr;
    QString textString;

    QByteArray ba;
    const char *c_str;
    int m;
    /***************文本中写入str1******************************************/
    for(; i<50 ; i++)
    {
        res = Device_Register_Read(deviceId, i, dataStr);
        m = dataStr.toInt();
        QString tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
        textString.append(tmpData);
        qDebug()<<" the data =  "<<m<<"    sixteen's num ="<< tmpData<<endl;

    }
    QFile file(filePath);
    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    QTextStream out(&file);
    out<<textString.toLocal8Bit()<<endl;


    /***************文本中写入str2*****************************************************/
//    res = System_Register_Read(17, dataStr);
////    ba = dataStr.toLatin1();
////    c_str = ba.data();
////    m = uint8_t(c_str[0]);
//    m = dataStr.toInt();
//    QString tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//    textString = tmpData;

//    res = System_Register_Read(18, dataStr);
////    ba = dataStr.toLatin1();
////    c_str = ba.data();
////    m = uint8_t(c_str[0]);
//    m = dataStr.toInt();
//    tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//    textString.append(tmpData);


//    res = System_Register_Read(226, dataStr);
////    ba = dataStr.toLatin1();
////    c_str = ba.data();
////    m = uint8_t(c_str[0]);
//    m = dataStr.toInt();
//    tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
//    textString.append(tmpData);




    QString tmpData ;
    textString.clear();
    for(int n=0; n<6; n++)
    {
        res = System_Register_Read(17+n,dataStr);
        m = dataStr.toInt();
        tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
        textString.append(tmpData);

    }
    out<<textString.toLocal8Bit()<<endl;


    /***************文本中写入str3*****************************************************/
    textString.clear();
    for (i = 0; i < 13; i++)
    {
        res = System_Register_Read((32+i), dataStr);
//        ba = dataStr.toLatin1();
//        c_str = ba.data();
//        m = uint8_t(c_str[0]);
        m = dataStr.toInt();
        tmpData = QString("%1 ").arg(m,2,16,QLatin1Char('0')).toUpper();
        textString.append(tmpData);
    }
    out<<textString.toLocal8Bit()<<endl;



    file.close();
    if(recvFlag)
    {
        isRecvFlag = true;
        qDebug()<<"saveSettingSlot has set the recvFlag =  true"<<endl;
        read_usb();


    }

    if(res)
    {
        linkInfoSignal(10);
        emit showRunInfoSignal(QStringLiteral("save setting file success"));
    }else
    {
        linkInfoSignal(11);
    }




}



/**************************串口相关***************************************/
void ReceUSB_Msg::openSerial_slot(Settings serialSetting, bool openFlag)
{
    if(NULL == serial)
    {
        serial = new QSerialPort();
        connect(serial,SIGNAL(readyRead()),this,SLOT(recvSerial_slot()));
    }

    if(openFlag)   //打开串口
    {
        serial->setPortName(serialSetting.name);
        serial->setBaudRate(serialSetting.baudRate);
        serial->setDataBits(serialSetting.dataBits);
        serial->setParity(serialSetting.parity);
        serial->setStopBits(serialSetting.stopBits);
        serial->setFlowControl(serialSetting.flowControl);
        if(serial->open(QIODevice::ReadWrite))
        {
           emit linkInfoSignal(16);
        }else
        {
           emit linkInfoSignal(17);
        }
        qDebug()<<"recvData = "<<serialSetting.name<<" -- "<<serialSetting.baudRate<<endl;

    }else       //关闭串口
    {
        qDebug()<<QStringLiteral("接收到关闭串口的信号")<<endl;
        serial->close();
    }


}



//接收串口发送过来的数据
void ReceUSB_Msg::recvSerial_slot()
{
    QByteArray temp = serial->readAll();
    QString strHex;//16进制数据

    if (!temp.isEmpty())
    {
        QDataStream out(&temp,QIODevice::ReadWrite);
        while (!out.atEnd())
        {
            qint8 outChar=0;
            out>>outChar;
            QString str=QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));

            if (str.length()>1)
            {
                strHex+=str+" ";
            }
            else
            {
                strHex+="0"+str+" ";
            }
        }
        strHex = strHex.toUpper();
        m_buffer.append(strHex);

//        qDebug()<<"m_buffer = "<<m_buffer<<endl;

        int totallen = m_buffer.size();
        while(totallen)
        {
            if(totallen <(1029*3))    //一个报文至少要有1029个字节,加上空格 1029*3
                return;

           int indexOf5A = m_buffer.indexOf("5A 83",0);
           if(indexOf5A < 0)  //没有找到5A 83
           {
               qDebug()<<"msg maybe error,can't find 5A     "<<"index ="<<indexOf5A<<"buffer"<<m_buffer<<endl;
               return;
           }else if(indexOf5A>0)  //第一次的时候前面会有冗余数据，删掉
           {
               m_buffer = m_buffer.right(totallen-indexOf5A);
               totallen = m_buffer.size();
               qDebug()<<"the first msg ,has some Redundant data,  indexOf5A="<<indexOf5A<<"  the left msg = "<<m_buffer<<endl;
               if(totallen <(1029*3))
                   return;
           }

           /*********************************/
           int length = (m_buffer.mid(9,2).toInt(NULL,16)*256 + m_buffer.mid(6,2).toInt(NULL,16) +5)*3;    //数据长度1024 + 命令字节数5
//           qDebug()<<"the  single Data  = "<<length<<endl;
           if(m_buffer.size()<length)    //不够一个命令的长度 返回
               return;
           QString single_Data = m_buffer.left(length);       //接收到的一整个数据包，交给函数处理
//           qDebug()<<"single_Data = "<<single_Data<<endl;

           //数据校验
           if(!msgCheck(single_Data))
           {
               qDebug()<<QStringLiteral("数据校验失败,singleData = ")<<single_Data<<endl;
               m_buffer = m_buffer.right(totallen-2);
               totallen = m_buffer.length();
               return;
           }

           //数据处理
           if(0 == ProtocolFlag )
               singleDataDeal(single_Data);
           if(1 == ProtocolFlag )
               dataDeal_2_256(single_Data);
           if(2 == ProtocolFlag)
               dataDeal_4_256(single_Data);

           m_buffer = m_buffer.right(totallen - length);
           totallen = m_buffer.size();

        } //while

   }
}

//把单包数据解析成4个指令，以信号的形式发送给数据处理线程，这么做是为了使接口统一
void ReceUSB_Msg::singleDataDeal(QString singleData)
{
    QByteArray sendArray;

    int len = singleData.length();

    QString dataStr = singleData.right( len - 4*3 );

    //把QString类型 转换为QByteArray,并向数据处理线程发送信号
    QString data1 = "09 00 00 00" + dataStr.mid(0, 64*4*3);
    data1 = data1.replace(" ","");
    sendArray = stringToByte(data1);

//    qDebug()<<" data1 ="<<data1.mid(0,20);
    emit recvMsgSignal(sendArray);


    QString data2 = "09 00 01 00 " + dataStr.mid(64*4*3,64*4*3);
    data2 = data2.replace(" ","");
    sendArray = stringToByte(data2);
    emit recvMsgSignal(sendArray);



    QString data3 = "09 00 02 00" + dataStr.mid(64*4*3*2,64*4*3);
    data3 = data3.replace(" ","");
    sendArray = stringToByte(data3);
    emit recvMsgSignal(sendArray);



    QString data4 = "09 00 03 00 " + dataStr.mid(64*4*3*3,64*4*3);
    data4 = data4.replace(" ","");
    sendArray = stringToByte(data4);
    emit recvMsgSignal(sendArray);
}

//2*256的解析,直接把数据转换成字节类型，以信号的形式发送给数据处理线程 完毕
void ReceUSB_Msg::dataDeal_2_256(QString Data)   //2x256协议的解析
{
    qDebug()<<"dataDeal_2_256 Data'size = "<<Data.size();
}



//4x256的解析
void ReceUSB_Msg::dataDeal_4_256(QString Data)   //4x256协议的解析
{
    Data.replace(" ","");                        //转换之前把空格去掉
//    qDebug()<<"before Data = "<<Data<<endl;
    Data = Data.mid(8,4096*2);
//    qDebug()<<"after Data = "<<Data<<endl;      //把包头和包尾的部分去掉
    QByteArray array = stringToByte(Data);       //转换成字节数据，发送给数据处理线程
    qDebug()<<"dataDeal_4_256 Data'size = "<<array.size();
    emit recvSerialSignal_4_256(array);
}






//工具函数
//QString to byte
QByteArray ReceUSB_Msg::stringToByte(QString str)
{
    QByteArray byte_arr;
    bool ok;
    //如果str的长度 不是2的倍数  那么直接返回空
    if(str.size()%2!=0){
        return QByteArray::fromHex("字符串不符合标准");
    }
    int len=str.size();
    for(int i=0;i<len;i+=2){
         byte_arr.append(char(str.mid(i,2).toUShort(&ok,16)));
    }


    /*****************验证数据的对错**********************************/
//    QDataStream out(&byte_arr,QIODevice::ReadWrite);
//    QString strHex;
//    while (!out.atEnd())
//    {
//        qint8 outChar=0;
//        out>>outChar;
//        QString str=QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));

//        if (str.length()>1)
//        {
//            strHex+=str+" ";
//        }
//        else
//        {
//            strHex+="0"+str+" ";
//        }
//    }
//    strHex = strHex.toUpper();

//    qDebug()<<QStringLiteral("发送的原始数据为：")<<strHex<<endl;
    /*********************************************************************/
    return byte_arr;
}

bool ReceUSB_Msg::msgCheck(QString strMsg)
{
    strMsg = strMsg.replace(" ","");

    int len = strMsg.length();
    int num = 0;
    for(int i=2; i<len-2; i+=2)  //命令区到数据区求和
        num += strMsg.mid(i,2).toInt(NULL,16);

    if(quint8(~num) == strMsg.right(2).toInt(NULL,16))
        return true;
    else
        return false;
}

