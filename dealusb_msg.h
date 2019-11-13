#ifndef DEALUSB_MSG_H
#define DEALUSB_MSG_H

#include <QObject>
#include<QImage>
#include<vector>
#include<QTimer>

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
    int imageArray[4][256];         //存储的是Tof的值
    int imageArray_peak[4][256];    //存储的是PEAK的值
    int lastSpadNum;

    /**********显示实时TOF和PEAK图示相关****************/
    bool isShowImageFlag;
    QStringList tofList;
    QStringList peakList;


    //读取本地文件相关
    QString filePath;
    QTimer *localFile_timer;
    int fileIndex;

    //重新计算角度版本时 垂直角度的值 默认设置为100
    int vexAngleValue;



    //根据强度值滤波 的offset ，以及滑动平均的帧数 默认设置为1
    int peakOffset;   //0
    int slideFrameNum;   //1

signals:

    void statisticsValueSignal(float,float,float,float);

    void saveTXTSignal(QString );

    void tofPeakImageSignal(QStringList,QStringList,int);    //tof peak num:1、2、4

public slots:

    void recvMsgSlot(QByteArray array);

    void recvMsgSlot_2_256(QByteArray array);

    void recvMsgSlot_4_256(QByteArray array);

    void alterStatisticFrameNum_slot(int num,bool isStartFlag);

    void showSettingParaSlot(int,int,int);

    void changeTofPeak_slot();

    void changePeakOffsetAverageFrame_slot(int offset_peak,int slideAverage_num);    // 设定peak的阈值（默认为0），peak的值小于阈值时，tof修改为65535 ； 并设定滑动平均的帧数



    /************串口相关的槽函数************/
    void recvSerialSlot_4_256(QByteArray MyBuffer);


    //添加读取本地tof和PEAK的槽函数
    void readLocalPCDFile();
    void selectLocalFile_slot(QString sPath);


};





#endif // DEALUSB_MSG_H
