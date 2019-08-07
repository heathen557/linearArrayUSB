#ifndef DEALUSB_MSG_H
#define DEALUSB_MSG_H

#include <QObject>
#include<QImage>

class DealUsb_msg : public QObject
{
    Q_OBJECT
public:
    explicit DealUsb_msg(QObject *parent = 0);

    QByteArray recvArray;

    int lastSpadNum;


    int cloudIndex;

    float  LSB ; //时钟频率
    bool isFirstLink;   //因为USB驱动存在问题，故设次函数

    float tofMin,tofMax,peakMin,peakMax,xMin,xMax,yMin,yMax,zMin,zMax;
    float temp_x,temp_y,temp_z;

    int r,g,b,rgb;

    int tmp_tofInfo[16384];
    int tmp_peakInfo[16384];

    QString tofPeakToSave_string;   //存储文件所需的tof和peak;



signals:
    void staticValueSignal(float,float,float,float,float,float,float,float,float,float);


    void saveTXTSignal(QString );

public slots:
    void recvMsgSlot(QByteArray array);


};

#endif // DEALUSB_MSG_H
