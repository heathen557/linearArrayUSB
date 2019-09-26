#include "calmeanstdthread.h"

//统计 均值和方差 相关
QMutex statisticMutex;
vector<vector<int>> allStatisticTofPoints;   //用于统计 均值和方差的 容器
vector<vector<int>> allStatisticPeakPoints;   //用于统计 均值和方差的 容器


calMeanStdThread::calMeanStdThread(QObject *parent) : QObject(parent)
{
    updateTimer  = NULL;


//    updateTimer.start(1000);   //1 sec刷新一次

}

void calMeanStdThread::startStop_slot(int flag)
{
    if(NULL == updateTimer)
    {
        updateTimer = new QTimer();
        connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateSlot()));
    }

    if(1 == flag)
    {
        updateTimer->start(5);
    }else {
        updateTimer->stop();
    }
}


void calMeanStdThread::updateSlot()      //刷新均值和方差的槽函数
{
    statisticMutex.lock();
    useStatisticTofPoints = allStatisticTofPoints;
    useStatisticPeakPoints = allStatisticPeakPoints;
    statisticMutex.unlock();

    tofMean_stringlist.clear();
    tofStd_stringlist.clear();
    peakMean_stringlist.clear();
    peakStd_stringlist.clear();


//    qDebug()<<QStringLiteral("显示时，查询到的每个点对应的个数为： ")<< useStatisticTofPoints[0].size()<<endl;
    for(int i =0; i<256; i++)
    {
        frameSize = useStatisticTofPoints[i].size();

        if(frameSize>0)                              //理论上应该与设置的帧数相等，此处是为了防止除数为零时引起程序异常；
        {
            //tof的均值 标准差的统计
            tofMean = std::accumulate(std::begin(useStatisticTofPoints[i]),std::end(useStatisticTofPoints[i]),0.0)/frameSize;
            tofAccum = 0.0;
            std::for_each (std::begin(useStatisticTofPoints[i]), std::end(useStatisticTofPoints[i]), [&](const double d) {
                    tofAccum  += (d-tofMean)*(d-tofMean);
                });
            tofStd = sqrt(tofAccum/(frameSize-1));


            //peak的均值 标准差的统计
            peakMean = std::accumulate(std::begin(useStatisticPeakPoints[i]),std::end(useStatisticPeakPoints[i]),0.0)/frameSize;
            peakAccum = 0.0;
            std::for_each (std::begin(useStatisticPeakPoints[i]), std::end(useStatisticPeakPoints[i]), [&](const double d) {
                    peakAccum  += (d-peakMean)*(d-peakMean);
                });
            peakStd = sqrt(peakAccum/(frameSize-1));


            tofMean_stringlist.append(QString::number(tofMean));
            tofStd_stringlist.append(QString::number(tofStd));
            peakMean_stringlist.append(QString::number(peakMean));
            peakStd_stringlist.append(QString::number(peakStd));
        }
    }


    emit statistic_MeanStdSignal(tofMean_stringlist,tofStd_stringlist,peakMean_stringlist,peakStd_stringlist);
}
