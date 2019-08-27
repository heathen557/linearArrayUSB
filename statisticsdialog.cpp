#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"


//QMutex statisticMutex;
//vector<vector<int>> allStatisticTofPoints;   //用于统计 均值和方差的 容器  TOF
//vector<vector<int>> allStatisticPeakPoints;   //用于统计 均值和方差的 容器  TOF

statisticsDialog::statisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::statisticsDialog)
{
    ui->setupUi(this);

    initCustomPlot();
}

void statisticsDialog::initCustomPlot()
{
    ui->TOFMean_widget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->TOFSTD_widget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->PEAKMEAN_widget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->PEAKSTD_widget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);


    ui->TOFMean_widget->legend->setVisible(true);
    ui->TOFMean_widget->legend->setFont(QFont("Helvetica", 9));
    ui->TOFMean_widget->yAxis->setRange(0,500);
//    ui->TOFMean_widget->yAxis->setLabel("mean");
//    ui->TOFMean_widget->yAxis2->setRange(0,1);
//    ui->TOF_widget->yAxis2->setVisible(true);
    ui->TOFMean_widget->xAxis->setRange(0,256);
//    ui->TOFMean_widget->xAxis->setLabel("point");
    ui->TOFMean_widget->addGraph();
    ui->TOFMean_widget->graph(0)->setPen(QPen(Qt::red));
    ui->TOFMean_widget->graph(0)->setName(QStringLiteral("均值"));

    ui->TOFSTD_widget->legend->setVisible(true);
    ui->TOFSTD_widget->legend->setFont(QFont("Helvetica", 9));
    ui->TOFSTD_widget->yAxis->setRange(-1,3);
    ui->TOFSTD_widget->xAxis->setRange(0,256);
    ui->TOFSTD_widget->addGraph();
    ui->TOFSTD_widget->graph(0)->setName(QStringLiteral("标准差"));

    ui->PEAKMEAN_widget->legend->setVisible(true);
    ui->PEAKMEAN_widget->legend->setFont(QFont("Helvetica", 9));
    ui->PEAKMEAN_widget->yAxis->setRange(0,500);
    ui->PEAKMEAN_widget->xAxis->setRange(0,256);
    ui->PEAKMEAN_widget->addGraph();
    ui->PEAKMEAN_widget->graph(0)->setPen(QPen(Qt::red));
    ui->PEAKMEAN_widget->graph(0)->setName(QStringLiteral("均值"));

    ui->PEAKSTD_widget->legend->setVisible(true);
    ui->PEAKSTD_widget->legend->setFont(QFont("Helvetica", 9));
    ui->PEAKSTD_widget->yAxis->setRange(-1,3);
    ui->PEAKSTD_widget->xAxis->setRange(0,256);
    ui->PEAKSTD_widget->addGraph();
    ui->PEAKSTD_widget->graph(0)->setName(QStringLiteral("标准差"));



}

statisticsDialog::~statisticsDialog()
{
    delete ui;
}


//开始统计的槽函数
void statisticsDialog::on_start_pushButton_clicked()
{
    if(QStringLiteral("开始统计") == ui->start_pushButton->text())
    {
        qDebug()<<"this is begin to start "<<endl;
        ui->start_pushButton->setText(QStringLiteral("结束统计"));
        int frameNumber = ui->frame_lineEdit->text().toInt();
        emit alterStatisticFrameNum_signal(frameNumber);
        emit startStop_signal(1);
    }else
    {
        emit startStop_signal(0);
        ui->start_pushButton->setText(QStringLiteral("开始统计"));
    }


//    int frameNumber = ui->frame_lineEdit->text().toInt();
//    emit alterStatisticFrameNum_signal(frameNumber);

}


//刷新均值和方差的槽函数
void statisticsDialog::statistic_MeanStdSlot(QStringList tofMeanStringlist,QStringList tofStdStringlist,QStringList peakMeanStringlist,QStringList peakStdStringlist)
{
    QVector<double> label_x(256);
    QVector<double> tofMean_y(256);
    QVector<double> tofStd_y(256);
    QVector<double> peakMean_y(256);
    QVector<double> peakStd_y(256);

    qDebug()<<"tofMeanStringlist.size() = "<<tofMeanStringlist.size()<<endl;

    QString str;
    for(int i=0; i<tofMeanStringlist.size(); i++)
    {
        label_x[i] = i;
        tofMean_y[i] = tofMeanStringlist[i].toDouble();
        tofStd_y[i] = tofStdStringlist[i].toDouble();
        peakMean_y[i] = peakMeanStringlist[i].toDouble();
        peakStd_y[i] = peakStdStringlist[i].toDouble();
    }

    ui->TOFMean_widget->graph(0)->addData(label_x,tofMean_y);
    ui->TOFMean_widget->replot();

    ui->TOFSTD_widget->graph(0)->addData(label_x,tofStd_y);
    ui->TOFSTD_widget->replot();

    ui->PEAKMEAN_widget->graph(0)->addData(label_x,peakMean_y);
    ui->PEAKMEAN_widget->replot();

    ui->PEAKSTD_widget->graph(0)->addData(label_x,peakStd_y);
    ui->PEAKSTD_widget->replot();

//    statisticMutex.lock();
//    useStatisticTofPoints = allStatisticTofPoints;
//    useStatisticPeakPoints = allStatisticPeakPoints;
//    statisticMutex.unlock();

//    tofMean_string.clear();
//    tofStd_string.clear();
//    peakMean_string.clear();
//    peakStd_string.clear();

//    for(int i =0; i<256; i++)
//    {
//        frameSize = useStatisticTofPoints[i].size();
//        if(frameSize>0)                              //理论上应该与设置的帧数相等，此处是为了防止除数为零时引起程序异常；
//        {
//            //tof的均值 标准差的统计
//            tofMean = std::accumulate(std::begin(useStatisticTofPoints[i]),std::end(useStatisticTofPoints[i]),0.0)/frameSize;
//            tofAccum = 0.0;
//            std::for_each (std::begin(useStatisticTofPoints[i]), std::end(useStatisticTofPoints[i]), [&](const double d) {
//                    tofAccum  += (d-tofMean)*(d-tofMean);
//                });
//            tofStd = sqrt(tofAccum/(frameSize-1));


//            //peak的均值 标准差的统计
//            peakMean = std::accumulate(std::begin(useStatisticPeakPoints[i]),std::end(useStatisticPeakPoints[i]),0.0)/frameSize;
//            peakAccum = 0.0;
//            std::for_each (std::begin(useStatisticPeakPoints[i]), std::end(useStatisticPeakPoints[i]), [&](const double d) {
//                    peakAccum  += (d-peakMean)*(d-peakMean);
//                });
//            peakStd = sqrt(peakAccum/(frameSize-1));

//            if( 0 == (i%10))
//            {
//                tofMean_string.append("\n");
//                tofStd_string.append("\n");
//                peakMean_string.append("\n");
//                peakStd_string.append("\n");
//            }


////            tofMean_string.append(QString::number(tofMean)).append("  ");
////            tofStd_string.append(QString::number(tofStd)).append("  ");

////            peakMean_string.append(QString::number(peakMean)).append("  ");
////            peakStd_string.append(QString::number(peakStd)).append("  ");

//            tofMean_x[i] = i;
//            tofMean_y[i] = tofMean;

//            tofStd_x[i] = i;
//            tofStd_y[i] = peakStd;


//        }

//    }

//    ui->TOF_widget->graph(0)->addData(tofMean_x,tofMean_y);
//    ui->TOF_widget->graph(1)->addData(tofStd_x,tofStd_y);

//    ui->TOF_widget->replot();
//    ui->tofMean_textEdit->setText(tofMean_string);
//    ui->tofStd_textEdit->setText(tofStd_string);
//    ui->peakMean_textEdit->setText(peakMean_string);
//    ui->peakStd_textEdit->setText(peakStd_string);

}
