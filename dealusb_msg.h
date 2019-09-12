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

    QString tofPeakToSave_string;   //存储文件所需的tof和peak;  储存排序以后的数据，
    QString tmpTofPeak_string[256*4];



    vector<float> Rece_points;        //保存一帧的点，先角度 后TOF

    int pointIndex;

    float angle;

    bool statisticStartFlag;
    int statisticIndex;
    int statisticFrameNumber;
    vector<vector<int>> tempStatisticTofPoints;   //用于统计 均值和方差的 容器
    vector<vector<int>> tempStatisticPeakPoints;   //用于统计 均值和方差的 容器



    int showFrameNum;
    int showAngle;
    int showTOFmax;

    bool isTOF_flag;



    /************ 2 * 256 / 4*256 相关*******************/
    int imageArray[4][256];
    int lastSpadNum;


signals:

    void statisticsValueSignal(float,float,float,float);

    void saveTXTSignal(QString );

public slots:

    void recvMsgSlot(QByteArray array);

    void recvMsgSlot_2_256(QByteArray array);

    void recvMsgSlot_4_256(QByteArray array);

    void alterStatisticFrameNum_slot(int num);

    void showSettingParaSlot(int,int,int);

    void  changeTofPeak_slot();


};

#endif // DEALUSB_MSG_H
