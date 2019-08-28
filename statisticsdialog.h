#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include<numeric>
#include<vector>
#include<QMutex>
#include<QTimer>
using namespace std;

namespace Ui {
class statisticsDialog;
}

class statisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit statisticsDialog(QWidget *parent = 0);
    ~statisticsDialog();
    float tofMean;
    float tofAccum;
    float tofStd;

    float peakMean;
    float peakAccum;
    float peakStd;

    int frameSize;

    vector<vector<int>> useStatisticTofPoints;
    vector<vector<int>> useStatisticPeakPoints;



    QString tofMean_string;
    QString tofStd_string;

    QString peakMean_string;
    QString peakStd_string;



private slots:
    void on_start_pushButton_clicked();

//    void updateSlot();      //刷新均值和方差的槽函数

    void initCustomPlot();

    void statistic_MeanStdSlot(QStringList,QStringList,QStringList,QStringList);

signals:
    void alterStatisticFrameNum_signal(int);

    void startStop_signal(int);

private:
    Ui::statisticsDialog *ui;
};

#endif // STATISTICSDIALOG_H
