#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"


QMutex statisticMutex;
vector<vector<int>> allStatisticTofPoints;   //用于统计 均值和方差的 容器  TOF
vector<vector<int>> allStatisticPeakPoints;   //用于统计 均值和方差的 容器  TOF

statisticsDialog::statisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::statisticsDialog)
{
    ui->setupUi(this);
    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(updateSlot()));
}

statisticsDialog::~statisticsDialog()
{
    delete ui;
}


//开始统计的槽函数
void statisticsDialog::on_start_pushButton_clicked()
{
    int frameNumber = ui->frame_lineEdit->text().toInt();
    emit alterStatisticFrameNum_signal(frameNumber);
    updateTimer.start(1000);   //1 sec刷新一次
}


//刷新均值和方差的槽函数
void statisticsDialog::updateSlot()
{
    statisticMutex.lock();
    useStatisticTofPoints = allStatisticTofPoints;
    useStatisticPeakPoints = allStatisticPeakPoints;
    statisticMutex.unlock();

    tofMean_string.clear();
    tofStd_string.clear();
    peakMean_string.clear();
    peakStd_string.clear();

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


            tofMean_string.append(QString::number(tofMean)).append("  ");
            tofStd_string.append(QString::number(tofStd)).append("  ");
            peakMean_string.append(QString::number(peakMean)).append("  ");
            peakStd_string.append(QString::number(peakStd)).append("  ");
        }

    }

    ui->tofMean_textEdit->setText(tofMean_string);
    ui->tofStd_textEdit->setText(tofStd_string);
    ui->peakMean_textEdit->setText(peakMean_string);
    ui->peakStd_textEdit->setText(peakStd_string);

}
