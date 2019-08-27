#ifndef CALMEANSTDTHREAD_H
#define CALMEANSTDTHREAD_H

#include <QObject>
#include<QMutex>
#include<vector>
#include<QTimer>
#include<QDebug>
using namespace std;

class calMeanStdThread : public QObject
{
    Q_OBJECT
public:
    explicit calMeanStdThread(QObject *parent = 0);

    vector<vector<int>> useStatisticTofPoints;
    vector<vector<int>> useStatisticPeakPoints;

    QStringList tofMean_stringlist;
    QStringList tofStd_stringlist;
    QStringList peakMean_stringlist;
    QStringList peakStd_stringlist;
    float tofMean;
    float tofAccum;
    float tofStd;
    float peakMean;
    float peakAccum;
    float peakStd;
    int frameSize;

    QTimer *updateTimer;

signals:

    void statistic_MeanStdSignal(QStringList,QStringList,QStringList,QStringList);

public slots:
    void updateSlot();      //刷新均值和方差的槽函数

    void startStop_slot(int );
};

#endif // CALMEANSTDTHREAD_H
