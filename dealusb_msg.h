#ifndef DEALUSB_MSG_H
#define DEALUSB_MSG_H

#include <QObject>
#include<QImage>
#include<vector>

using namespace std;

class DealUsb_msg : public QObject
{
    Q_OBJECT

public:

    explicit DealUsb_msg(QObject *parent = 0);

    QByteArray recvArray;

    int lastLineNum;

    float tofMin,tofMax,peakMin,peakMax;

    QString tofPeakToSave_string;   //存储文件所需的tof和peak;

    vector<int> Rece_points;        //保存一帧的点，先角度 后TOF

    int pointIndex;

    float angle;

signals:

    void statisticsValueSignal(float,float,float,float);

    void saveTXTSignal(QString );

public slots:

    void recvMsgSlot(QByteArray array);


};

#endif // DEALUSB_MSG_H
