#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"


statisticsDialog::statisticsDialog(QWidget *parent) :
    ui(new Ui::statisticsDialog),QDialog(parent),tofImage(256,32, QImage::Format_RGB32),peakImage(256,32, QImage::Format_RGB32)
{
    ui->setupUi(this);
    gainImage = 1;

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
        emit alterStatisticFrameNum_signal(frameNumber,true);
        emit startStop_signal(1);
    }else
    {
        int frameNumber = ui->frame_lineEdit->text().toInt();
        emit alterStatisticFrameNum_signal(frameNumber,false);
        emit startStop_signal(0);
        ui->start_pushButton->setText(QStringLiteral("开始统计"));
    }

//    qDebug()<<"begin stats"<<endl;
//    float width_scale = ui->tofImage_label->width()/double(tofImage.width());
//    float height_scale = ui->tofImage_label->height()/double(tofImage.height());
//    tofImage.fill(0);
//    peakImage.fill(0);

//    for(int i=0;i<256;i++)
//    {
//        //设置TOF图像、强度图像的颜色
//        tof = 50;
//        intensity = 1000;
//        gainIndex_tof = tof*gainImage;
//        gainIndex_intensity =intensity * gainImage;

//        if(gainIndex_tof<1024 && gainIndex_tof>=0)
//            tofColor = qRgb(colormap[gainIndex_tof * 3], colormap[gainIndex_tof * 3 + 1], colormap[gainIndex_tof * 3 + 2]);
//        else
//            tofColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

//        if(gainIndex_intensity<1024 && gainIndex_intensity>=0)
//            intenColor = qRgb(colormap[gainIndex_intensity * 3], colormap[gainIndex_intensity * 3 + 1], colormap[gainIndex_intensity * 3 + 2]);
//        else
//            intenColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

//        int rowNum = tofImage.height()/2;
//        tofImage.setPixel(i,rowNum-2,tofColor);
//        tofImage.setPixel(i,rowNum-1,intenColor);
//        tofImage.setPixel(i,rowNum,tofColor);
//        tofImage.setPixel(i,rowNum+1,intenColor);
//        peakImage.setPixel(i,rowNum,intenColor);
//    }

//    resTofImage = tofImage.scaled(tofImage.width()*width_scale, tofImage.height()*height_scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    resPeakImage = peakImage.scaled(peakImage.width()*width_scale, peakImage.height()*height_scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    QPixmap pixmap(QPixmap::fromImage (resTofImage));
//    ui->tofImage_label->setPixmap(pixmap);
//    QPixmap pixmap2(QPixmap::fromImage (resPeakImage));
//    ui->peakImage_label->setPixmap(pixmap2);
//    qDebug()<<"end stats"<<endl;


}


//刷新均值和方差的槽函数
void statisticsDialog::statistic_MeanStdSlot(QStringList tofMeanStringlist,QStringList tofStdStringlist,QStringList peakMeanStringlist,QStringList peakStdStringlist)
{
    QVector<double> label_x(256);
    QVector<double> tofMean_y(256);
    QVector<double> tofStd_y(256);
    QVector<double> peakMean_y(256);
    QVector<double> peakStd_y(256);

    QString str;
    for(int i=0; i<tofMeanStringlist.size(); i++)
    {
        label_x[i] = i;
        tofMean_y[i] = tofMeanStringlist[i].toDouble();
        tofStd_y[i] = tofStdStringlist[i].toDouble();
        peakMean_y[i] = peakMeanStringlist[i].toDouble();
        peakStd_y[i] = peakStdStringlist[i].toDouble();

    }

    ui->TOFMean_widget->graph(0)->setData(label_x,tofMean_y);
    ui->TOFMean_widget->replot();

    ui->TOFSTD_widget->graph(0)->setData(label_x,tofStd_y);
    ui->TOFSTD_widget->replot();

    ui->PEAKMEAN_widget->graph(0)->setData(label_x,peakMean_y);
    ui->PEAKMEAN_widget->replot();

    ui->PEAKSTD_widget->graph(0)->setData(label_x,peakStd_y);
    ui->PEAKSTD_widget->replot();


    showImage();
}

//接收tof和peak的信息， 以供显示二维图像的槽函数
void statisticsDialog::tofPeakImageSlot(QStringList tof_list,QStringList peak_list,int rowNum)
{
//    qDebug()<<"tofList's len = "<<tof_list.length()<<"    rowNum = "<<rowNum<<endl;
    tofStringList = tof_list;
    peakStringList = peak_list;
    allRowNum = rowNum;


}

//显示TOF/PEAK图像的函数
void statisticsDialog::showImage()
{
    float width_scale = ui->tofImage_label->width()/double(tofImage.width());
    float height_scale = ui->tofImage_label->height()/double(tofImage.height());
    tofImage.fill(0);
    peakImage.fill(0);

    if(1 == allRowNum)
    {
        for(int i=0;i<tofStringList.length();i++)      //256
        {
            //设置TOF图像、强度图像的颜色
            tof = tofStringList[i].toInt();
            intensity = peakStringList[i].toInt();
            gainIndex_tof = tof*gainImage;
            gainIndex_intensity =intensity * gainImage;

            if(gainIndex_tof<1024 && gainIndex_tof>=0)
                tofColor = qRgb(colormap[gainIndex_tof * 3], colormap[gainIndex_tof * 3 + 1], colormap[gainIndex_tof * 3 + 2]);
            else
                tofColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

            if(gainIndex_intensity<1024 && gainIndex_intensity>=0)
                intenColor = qRgb(colormap[gainIndex_intensity * 3], colormap[gainIndex_intensity * 3 + 1], colormap[gainIndex_intensity * 3 + 2]);
            else
                intenColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

            int allRowNum = tofImage.height()/2;
//            qDebug()<<" i= "<<i<<"  allRowNum="<<allRowNum<<endl;

            tofImage.setPixel(i,allRowNum,tofColor);
            peakImage.setPixel(i,allRowNum,intenColor);
        }

    }else if(2 == allRowNum)
    {
        for(int i=0;i<tofStringList.length();i++)      //512
        {
            //设置TOF图像、强度图像的颜色
            tof = tofStringList[i].toInt();
            intensity = peakStringList[i].toInt();
            gainIndex_tof = tof*gainImage;
            gainIndex_intensity =intensity * gainImage;

            if(gainIndex_tof<1024 && gainIndex_tof>=0)
                tofColor = qRgb(colormap[gainIndex_tof * 3], colormap[gainIndex_tof * 3 + 1], colormap[gainIndex_tof * 3 + 2]);
            else
                tofColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

            if(gainIndex_intensity<1024 && gainIndex_intensity>=0)
                intenColor = qRgb(colormap[gainIndex_intensity * 3], colormap[gainIndex_intensity * 3 + 1], colormap[gainIndex_intensity * 3 + 2]);
            else
                intenColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

            int allRowNum = tofImage.height()/2;
            int x_pix = (i<256) ? i:(i-256);
            int y_pix = (i<256) ? allRowNum:(allRowNum+1);
            tofImage.setPixel(x_pix,y_pix,tofColor);
            peakImage.setPixel(x_pix,y_pix,intenColor);

        }
    }else if(4 == allRowNum)
    {
        for(int i=0; i<tofStringList.length(); i++)
        {
            //设置TOF图像、强度图像的颜色
            tof = tofStringList[i].toInt();
            intensity = peakStringList[i].toInt();
            gainIndex_tof = tof*gainImage;
            gainIndex_intensity =intensity * gainImage;

            if(gainIndex_tof<1024 && gainIndex_tof>=0)
                tofColor = qRgb(colormap[gainIndex_tof * 3], colormap[gainIndex_tof * 3 + 1], colormap[gainIndex_tof * 3 + 2]);
            else
                tofColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

            if(gainIndex_intensity<1024 && gainIndex_intensity>=0)
                intenColor = qRgb(colormap[gainIndex_intensity * 3], colormap[gainIndex_intensity * 3 + 1], colormap[gainIndex_intensity * 3 + 2]);
            else
                intenColor = qRgb(colormap[1023 * 3], colormap[1023 * 3 + 1], colormap[1023 * 3 + 2]);

            int allRowNum = tofImage.height()/2;
            int x_pix,y_pix;
            if(i<256)
            {
                x_pix = i;
                y_pix = allRowNum - 2;
            }else if(i<512)
            {
                x_pix = i - 256;
                y_pix = allRowNum - 1;
            }else if(i<768)
            {
                x_pix = i - 512;
                y_pix = allRowNum;
            }else if(i<1024)
            {
                x_pix = i - 768;
                y_pix = allRowNum + 1;
            }

            tofImage.setPixel(x_pix,y_pix,tofColor);
            peakImage.setPixel(x_pix,y_pix,intenColor);
        }
    }

    resTofImage = tofImage.scaled(tofImage.width()*width_scale, tofImage.height()*height_scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    resPeakImage = peakImage.scaled(peakImage.width()*width_scale, peakImage.height()*height_scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap pixmap(QPixmap::fromImage (resTofImage));
    ui->tofImage_label->setPixmap(pixmap);
    QPixmap pixmap2(QPixmap::fromImage (resPeakImage));
    ui->peakImage_label->setPixmap(pixmap2);
}




//关闭窗口时发送的信号
void statisticsDialog::closeEvent(QCloseEvent *event)
{
    int frameNumber = ui->frame_lineEdit->text().toInt();
    emit alterStatisticFrameNum_signal(frameNumber,false);
    emit startStop_signal(0);
    ui->start_pushButton->setText(QStringLiteral("开始统计"));

}

void statisticsDialog::on_verticalSlider_sliderMoved(int position)
{
//    qDebug()<<" position = "<<position<<endl;
    gainImage = position;
    ui->verticalSlider_2->setValue(position);
}

void statisticsDialog::on_verticalSlider_2_sliderMoved(int position)
{
    gainImage = position;
    ui->verticalSlider->setValue(position);

}
