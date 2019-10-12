﻿#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include<numeric>
#include<vector>
#include<QMutex>
#include<QTimer>
#include<QImage>
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




    QImage tofImage;
    QImage resTofImage;
    QImage peakImage;
    QImage resPeakImage;
    int tof,intensity,gainIndex_tof,gainIndex_intensity,gainImage;
    QRgb tofColor,intenColor;
    QStringList tofStringList;
    QStringList peakStringList;
    int allRowNum;


    void showImage();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_start_pushButton_clicked();

//    void updateSlot();      //刷新均值和方差的槽函数

    void initCustomPlot();

    void statistic_MeanStdSlot(QStringList,QStringList,QStringList,QStringList);



    void tofPeakImageSlot(QStringList tof_list,QStringList peak_list,int rowNum);

    void on_verticalSlider_sliderMoved(int position);

    void on_verticalSlider_2_sliderMoved(int position);

signals:
    void alterStatisticFrameNum_signal(int,bool);     //统计帧数 以及 是否进行统计的信号

    void startStop_signal(int);

private:
    Ui::statisticsDialog *ui;
};

static double colormap[1024*3]={
         0,  0,129,
         0,  0,130,
         0,  0,131,
         0,  0,132,
         0,  0,133,
         0,  0,134,
         0,  0,135,
         0,  0,136,
         0,  0,137,
         0,  0,138,
         0,  0,139,
         0,  0,140,
         0,  0,141,
         0,  0,142,
         0,  0,143,
         0,  0,144,
         0,  0,145,
         0,  0,146,
         0,  0,147,
         0,  0,148,
         0,  0,149,
         0,  0,150,
         0,  0,151,
         0,  0,152,
         0,  0,153,
         0,  0,154,
         0,  0,155,
         0,  0,156,
         0,  0,157,
         0,  0,158,
         0,  0,159,
         0,  0,160,
         0,  0,161,
         0,  0,162,
         0,  0,163,
         0,  0,164,
         0,  0,165,
         0,  0,166,
         0,  0,167,
         0,  0,168,
         0,  0,169,
         0,  0,170,
         0,  0,171,
         0,  0,172,
         0,  0,173,
         0,  0,174,
         0,  0,175,
         0,  0,176,
         0,  0,177,
         0,  0,178,
         0,  0,179,
         0,  0,180,
         0,  0,181,
         0,  0,182,
         0,  0,183,
         0,  0,184,
         0,  0,185,
         0,  0,186,
         0,  0,187,
         0,  0,188,
         0,  0,189,
         0,  0,190,
         0,  0,191,
         0,  0,192,
         0,  0,193,
         0,  0,194,
         0,  0,195,
         0,  0,196,
         0,  0,197,
         0,  0,198,
         0,  0,199,
         0,  0,200,
         0,  0,201,
         0,  0,202,
         0,  0,203,
         0,  0,204,
         0,  0,205,
         0,  0,206,
         0,  0,207,
         0,  0,208,
         0,  0,209,
         0,  0,210,
         0,  0,211,
         0,  0,212,
         0,  0,213,
         0,  0,214,
         0,  0,215,
         0,  0,216,
         0,  0,217,
         0,  0,218,
         0,  0,219,
         0,  0,220,
         0,  0,221,
         0,  0,222,
         0,  0,223,
         0,  0,224,
         0,  0,225,
         0,  0,226,
         0,  0,227,
         0,  0,228,
         0,  0,229,
         0,  0,230,
         0,  0,231,
         0,  0,232,
         0,  0,233,
         0,  0,234,
         0,  0,235,
         0,  0,236,
         0,  0,237,
         0,  0,238,
         0,  0,239,
         0,  0,240,
         0,  0,241,
         0,  0,242,
         0,  0,243,
         0,  0,244,
         0,  0,245,
         0,  0,246,
         0,  0,247,
         0,  0,248,
         0,  0,249,
         0,  0,250,
         0,  0,251,
         0,  0,252,
         0,  0,253,
         0,  0,254,
         0,  0,255,
         0,  0,255,
         0,  1,255,
         0,  2,255,
         0,  3,255,
         0,  4,255,
         0,  5,255,
         0,  6,255,
         0,  7,255,
         0,  8,255,
         0,  9,255,
         0, 10,255,
         0, 11,255,
         0, 12,255,
         0, 13,255,
         0, 14,255,
         0, 15,255,
         0, 16,255,
         0, 17,255,
         0, 18,255,
         0, 19,255,
         0, 20,255,
         0, 21,255,
         0, 22,255,
         0, 23,255,
         0, 24,255,
         0, 25,255,
         0, 26,255,
         0, 27,255,
         0, 28,255,
         0, 29,255,
         0, 30,255,
         0, 31,255,
         0, 32,255,
         0, 33,255,
         0, 34,255,
         0, 35,255,
         0, 36,255,
         0, 37,255,
         0, 38,255,
         0, 39,255,
         0, 40,255,
         0, 41,255,
         0, 42,255,
         0, 43,255,
         0, 44,255,
         0, 45,255,
         0, 46,255,
         0, 47,255,
         0, 48,255,
         0, 49,255,
         0, 50,255,
         0, 51,255,
         0, 52,255,
         0, 53,255,
         0, 54,255,
         0, 55,255,
         0, 56,255,
         0, 57,255,
         0, 58,255,
         0, 59,255,
         0, 60,255,
         0, 61,255,
         0, 62,255,
         0, 63,255,
         0, 64,255,
         0, 65,255,
         0, 66,255,
         0, 67,255,
         0, 68,255,
         0, 69,255,
         0, 70,255,
         0, 71,255,
         0, 72,255,
         0, 73,255,
         0, 74,255,
         0, 75,255,
         0, 76,255,
         0, 77,255,
         0, 78,255,
         0, 79,255,
         0, 80,255,
         0, 81,255,
         0, 82,255,
         0, 83,255,
         0, 84,255,
         0, 85,255,
         0, 86,255,
         0, 87,255,
         0, 88,255,
         0, 89,255,
         0, 90,255,
         0, 91,255,
         0, 92,255,
         0, 93,255,
         0, 94,255,
         0, 95,255,
         0, 96,255,
         0, 97,255,
         0, 98,255,
         0, 99,255,
         0,100,255,
         0,101,255,
         0,102,255,
         0,103,255,
         0,104,255,
         0,105,255,
         0,106,255,
         0,107,255,
         0,108,255,
         0,109,255,
         0,110,255,
         0,111,255,
         0,112,255,
         0,113,255,
         0,114,255,
         0,115,255,
         0,116,255,
         0,117,255,
         0,118,255,
         0,119,255,
         0,120,255,
         0,121,255,
         0,122,255,
         0,123,255,
         0,124,255,
         0,125,255,
         0,126,255,
         0,127,255,
         0,128,255,
         0,129,255,
         0,130,255,
         0,131,255,
         0,132,255,
         0,133,255,
         0,134,255,
         0,135,255,
         0,136,255,
         0,137,255,
         0,138,255,
         0,139,255,
         0,140,255,
         0,141,255,
         0,142,255,
         0,143,255,
         0,144,255,
         0,145,255,
         0,146,255,
         0,147,255,
         0,148,255,
         0,149,255,
         0,150,255,
         0,151,255,
         0,152,255,
         0,153,255,
         0,154,255,
         0,155,255,
         0,156,255,
         0,157,255,
         0,158,255,
         0,159,255,
         0,160,255,
         0,161,255,
         0,162,255,
         0,163,255,
         0,164,255,
         0,165,255,
         0,166,255,
         0,167,255,
         0,168,255,
         0,169,255,
         0,170,255,
         0,171,255,
         0,172,255,
         0,173,255,
         0,174,255,
         0,175,255,
         0,176,255,
         0,177,255,
         0,178,255,
         0,179,255,
         0,180,255,
         0,181,255,
         0,182,255,
         0,183,255,
         0,184,255,
         0,185,255,
         0,186,255,
         0,187,255,
         0,188,255,
         0,189,255,
         0,190,255,
         0,191,255,
         0,192,255,
         0,193,255,
         0,194,255,
         0,195,255,
         0,196,255,
         0,197,255,
         0,198,255,
         0,199,255,
         0,200,255,
         0,201,255,
         0,202,255,
         0,203,255,
         0,204,255,
         0,205,255,
         0,206,255,
         0,207,255,
         0,208,255,
         0,209,255,
         0,210,255,
         0,211,255,
         0,212,255,
         0,213,255,
         0,214,255,
         0,215,255,
         0,216,255,
         0,217,255,
         0,218,255,
         0,219,255,
         0,220,255,
         0,221,255,
         0,222,255,
         0,223,255,
         0,224,255,
         0,225,255,
         0,226,255,
         0,227,255,
         0,228,255,
         0,229,255,
         0,230,255,
         0,231,255,
         0,232,255,
         0,233,255,
         0,234,255,
         0,235,255,
         0,236,255,
         0,237,255,
         0,238,255,
         0,239,255,
         0,240,255,
         0,241,255,
         0,242,255,
         0,243,255,
         0,244,255,
         0,245,255,
         0,246,255,
         0,247,255,
         0,248,255,
         0,249,255,
         0,250,255,
         0,251,255,
         0,252,255,
         0,253,255,
         0,254,255,
         0,255,255,
         0,255,255,
         1,255,255,
         2,255,254,
         3,255,253,
         4,255,252,
         5,255,251,
         6,255,250,
         7,255,249,
         8,255,248,
         9,255,247,
        10,255,246,
        11,255,245,
        12,255,244,
        13,255,243,
        14,255,242,
        15,255,241,
        16,255,240,
        17,255,239,
        18,255,238,
        19,255,237,
        20,255,236,
        21,255,235,
        22,255,234,
        23,255,233,
        24,255,232,
        25,255,231,
        26,255,230,
        27,255,229,
        28,255,228,
        29,255,227,
        30,255,226,
        31,255,225,
        32,255,224,
        33,255,223,
        34,255,222,
        35,255,221,
        36,255,220,
        37,255,219,
        38,255,218,
        39,255,217,
        40,255,216,
        41,255,215,
        42,255,214,
        43,255,213,
        44,255,212,
        45,255,211,
        46,255,210,
        47,255,209,
        48,255,208,
        49,255,207,
        50,255,206,
        51,255,205,
        52,255,204,
        53,255,203,
        54,255,202,
        55,255,201,
        56,255,200,
        57,255,199,
        58,255,198,
        59,255,197,
        60,255,196,
        61,255,195,
        62,255,194,
        63,255,193,
        64,255,192,
        65,255,191,
        66,255,190,
        67,255,189,
        68,255,188,
        69,255,187,
        70,255,186,
        71,255,185,
        72,255,184,
        73,255,183,
        74,255,182,
        75,255,181,
        76,255,180,
        77,255,179,
        78,255,178,
        79,255,177,
        80,255,176,
        81,255,175,
        82,255,174,
        83,255,173,
        84,255,172,
        85,255,171,
        86,255,170,
        87,255,169,
        88,255,168,
        89,255,167,
        90,255,166,
        91,255,165,
        92,255,164,
        93,255,163,
        94,255,162,
        95,255,161,
        96,255,160,
        97,255,159,
        98,255,158,
        99,255,157,
       100,255,156,
       101,255,155,
       102,255,154,
       103,255,153,
       104,255,152,
       105,255,151,
       106,255,150,
       107,255,149,
       108,255,148,
       109,255,147,
       110,255,146,
       111,255,145,
       112,255,144,
       113,255,143,
       114,255,142,
       115,255,141,
       116,255,140,
       117,255,139,
       118,255,138,
       119,255,137,
       120,255,136,
       121,255,135,
       122,255,134,
       123,255,133,
       124,255,132,
       125,255,131,
       126,255,130,
       127,255,129,
       128,255,128,
       129,255,127,
       130,255,126,
       131,255,125,
       132,255,124,
       133,255,123,
       134,255,122,
       135,255,121,
       136,255,120,
       137,255,119,
       138,255,118,
       139,255,117,
       140,255,116,
       141,255,115,
       142,255,114,
       143,255,113,
       144,255,112,
       145,255,111,
       146,255,110,
       147,255,109,
       148,255,108,
       149,255,107,
       150,255,106,
       151,255,105,
       152,255,104,
       153,255,103,
       154,255,102,
       155,255,101,
       156,255,100,
       157,255, 99,
       158,255, 98,
       159,255, 97,
       160,255, 96,
       161,255, 95,
       162,255, 94,
       163,255, 93,
       164,255, 92,
       165,255, 91,
       166,255, 90,
       167,255, 89,
       168,255, 88,
       169,255, 87,
       170,255, 86,
       171,255, 85,
       172,255, 84,
       173,255, 83,
       174,255, 82,
       175,255, 81,
       176,255, 80,
       177,255, 79,
       178,255, 78,
       179,255, 77,
       180,255, 76,
       181,255, 75,
       182,255, 74,
       183,255, 73,
       184,255, 72,
       185,255, 71,
       186,255, 70,
       187,255, 69,
       188,255, 68,
       189,255, 67,
       190,255, 66,
       191,255, 65,
       192,255, 64,
       193,255, 63,
       194,255, 62,
       195,255, 61,
       196,255, 60,
       197,255, 59,
       198,255, 58,
       199,255, 57,
       200,255, 56,
       201,255, 55,
       202,255, 54,
       203,255, 53,
       204,255, 52,
       205,255, 51,
       206,255, 50,
       207,255, 49,
       208,255, 48,
       209,255, 47,
       210,255, 46,
       211,255, 45,
       212,255, 44,
       213,255, 43,
       214,255, 42,
       215,255, 41,
       216,255, 40,
       217,255, 39,
       218,255, 38,
       219,255, 37,
       220,255, 36,
       221,255, 35,
       222,255, 34,
       223,255, 33,
       224,255, 32,
       225,255, 31,
       226,255, 30,
       227,255, 29,
       228,255, 28,
       229,255, 27,
       230,255, 26,
       231,255, 25,
       232,255, 24,
       233,255, 23,
       234,255, 22,
       235,255, 21,
       236,255, 20,
       237,255, 19,
       238,255, 18,
       239,255, 17,
       240,255, 16,
       241,255, 15,
       242,255, 14,
       243,255, 13,
       244,255, 12,
       245,255, 11,
       246,255, 10,
       247,255,  9,
       248,255,  8,
       249,255,  7,
       250,255,  6,
       251,255,  5,
       252,255,  4,
       253,255,  3,
       254,255,  2,
       255,255,  1,
       255,255,  0,
       255,255,  0,
       255,254,  0,
       255,253,  0,
       255,252,  0,
       255,251,  0,
       255,250,  0,
       255,249,  0,
       255,248,  0,
       255,247,  0,
       255,246,  0,
       255,245,  0,
       255,244,  0,
       255,243,  0,
       255,242,  0,
       255,241,  0,
       255,240,  0,
       255,239,  0,
       255,238,  0,
       255,237,  0,
       255,236,  0,
       255,235,  0,
       255,234,  0,
       255,233,  0,
       255,232,  0,
       255,231,  0,
       255,230,  0,
       255,229,  0,
       255,228,  0,
       255,227,  0,
       255,226,  0,
       255,225,  0,
       255,224,  0,
       255,223,  0,
       255,222,  0,
       255,221,  0,
       255,220,  0,
       255,219,  0,
       255,218,  0,
       255,217,  0,
       255,216,  0,
       255,215,  0,
       255,214,  0,
       255,213,  0,
       255,212,  0,
       255,211,  0,
       255,210,  0,
       255,209,  0,
       255,208,  0,
       255,207,  0,
       255,206,  0,
       255,205,  0,
       255,204,  0,
       255,203,  0,
       255,202,  0,
       255,201,  0,
       255,200,  0,
       255,199,  0,
       255,198,  0,
       255,197,  0,
       255,196,  0,
       255,195,  0,
       255,194,  0,
       255,193,  0,
       255,192,  0,
       255,191,  0,
       255,190,  0,
       255,189,  0,
       255,188,  0,
       255,187,  0,
       255,186,  0,
       255,185,  0,
       255,184,  0,
       255,183,  0,
       255,182,  0,
       255,181,  0,
       255,180,  0,
       255,179,  0,
       255,178,  0,
       255,177,  0,
       255,176,  0,
       255,175,  0,
       255,174,  0,
       255,173,  0,
       255,172,  0,
       255,171,  0,
       255,170,  0,
       255,169,  0,
       255,168,  0,
       255,167,  0,
       255,166,  0,
       255,165,  0,
       255,164,  0,
       255,163,  0,
       255,162,  0,
       255,161,  0,
       255,160,  0,
       255,159,  0,
       255,158,  0,
       255,157,  0,
       255,156,  0,
       255,155,  0,
       255,154,  0,
       255,153,  0,
       255,152,  0,
       255,151,  0,
       255,150,  0,
       255,149,  0,
       255,148,  0,
       255,147,  0,
       255,146,  0,
       255,145,  0,
       255,144,  0,
       255,143,  0,
       255,142,  0,
       255,141,  0,
       255,140,  0,
       255,139,  0,
       255,138,  0,
       255,137,  0,
       255,136,  0,
       255,135,  0,
       255,134,  0,
       255,133,  0,
       255,132,  0,
       255,131,  0,
       255,130,  0,
       255,129,  0,
       255,128,  0,
       255,127,  0,
       255,126,  0,
       255,125,  0,
       255,124,  0,
       255,123,  0,
       255,122,  0,
       255,121,  0,
       255,120,  0,
       255,119,  0,
       255,118,  0,
       255,117,  0,
       255,116,  0,
       255,115,  0,
       255,114,  0,
       255,113,  0,
       255,112,  0,
       255,111,  0,
       255,110,  0,
       255,109,  0,
       255,108,  0,
       255,107,  0,
       255,106,  0,
       255,105,  0,
       255,104,  0,
       255,103,  0,
       255,102,  0,
       255,101,  0,
       255,100,  0,
       255, 99,  0,
       255, 98,  0,
       255, 97,  0,
       255, 96,  0,
       255, 95,  0,
       255, 94,  0,
       255, 93,  0,
       255, 92,  0,
       255, 91,  0,
       255, 90,  0,
       255, 89,  0,
       255, 88,  0,
       255, 87,  0,
       255, 86,  0,
       255, 85,  0,
       255, 84,  0,
       255, 83,  0,
       255, 82,  0,
       255, 81,  0,
       255, 80,  0,
       255, 79,  0,
       255, 78,  0,
       255, 77,  0,
       255, 76,  0,
       255, 75,  0,
       255, 74,  0,
       255, 73,  0,
       255, 72,  0,
       255, 71,  0,
       255, 70,  0,
       255, 69,  0,
       255, 68,  0,
       255, 67,  0,
       255, 66,  0,
       255, 65,  0,
       255, 64,  0,
       255, 63,  0,
       255, 62,  0,
       255, 61,  0,
       255, 60,  0,
       255, 59,  0,
       255, 58,  0,
       255, 57,  0,
       255, 56,  0,
       255, 55,  0,
       255, 54,  0,
       255, 53,  0,
       255, 52,  0,
       255, 51,  0,
       255, 50,  0,
       255, 49,  0,
       255, 48,  0,
       255, 47,  0,
       255, 46,  0,
       255, 45,  0,
       255, 44,  0,
       255, 43,  0,
       255, 42,  0,
       255, 41,  0,
       255, 40,  0,
       255, 39,  0,
       255, 38,  0,
       255, 37,  0,
       255, 36,  0,
       255, 35,  0,
       255, 34,  0,
       255, 33,  0,
       255, 32,  0,
       255, 31,  0,
       255, 30,  0,
       255, 29,  0,
       255, 28,  0,
       255, 27,  0,
       255, 26,  0,
       255, 25,  0,
       255, 24,  0,
       255, 23,  0,
       255, 22,  0,
       255, 21,  0,
       255, 20,  0,
       255, 19,  0,
       255, 18,  0,
       255, 17,  0,
       255, 16,  0,
       255, 15,  0,
       255, 14,  0,
       255, 13,  0,
       255, 12,  0,
       255, 11,  0,
       255, 10,  0,
       255,  9,  0,
       255,  8,  0,
       255,  7,  0,
       255,  6,  0,
       255,  5,  0,
       255,  4,  0,
       255,  3,  0,
       255,  2,  0,
       255,  1,  0,
       255,  0,  0,
       255,  0,  0,
       254,  0,  0,
       253,  0,  0,
       252,  0,  0,
       251,  0,  0,
       250,  0,  0,
       249,  0,  0,
       248,  0,  0,
       247,  0,  0,
       246,  0,  0,
       245,  0,  0,
       244,  0,  0,
       243,  0,  0,
       242,  0,  0,
       241,  0,  0,
       240,  0,  0,
       239,  0,  0,
       238,  0,  0,
       237,  0,  0,
       236,  0,  0,
       235,  0,  0,
       234,  0,  0,
       233,  0,  0,
       232,  0,  0,
       231,  0,  0,
       230,  0,  0,
       229,  0,  0,
       228,  0,  0,
       227,  0,  0,
       226,  0,  0,
       225,  0,  0,
       224,  0,  0,
       223,  0,  0,
       222,  0,  0,
       221,  0,  0,
       220,  0,  0,
       219,  0,  0,
       218,  0,  0,
       217,  0,  0,
       216,  0,  0,
       215,  0,  0,
       214,  0,  0,
       213,  0,  0,
       212,  0,  0,
       211,  0,  0,
       210,  0,  0,
       209,  0,  0,
       208,  0,  0,
       207,  0,  0,
       206,  0,  0,
       205,  0,  0,
       204,  0,  0,
       203,  0,  0,
       202,  0,  0,
       201,  0,  0,
       200,  0,  0,
       199,  0,  0,
       198,  0,  0,
       197,  0,  0,
       196,  0,  0,
       195,  0,  0,
       194,  0,  0,
       193,  0,  0,
       192,  0,  0,
       191,  0,  0,
       190,  0,  0,
       189,  0,  0,
       188,  0,  0,
       187,  0,  0,
       186,  0,  0,
       185,  0,  0,
       184,  0,  0,
       183,  0,  0,
       182,  0,  0,
       181,  0,  0,
       180,  0,  0,
       179,  0,  0,
       178,  0,  0,
       177,  0,  0,
       176,  0,  0,
       175,  0,  0,
       174,  0,  0,
       173,  0,  0,
       172,  0,  0,
       171,  0,  0,
       170,  0,  0,
       169,  0,  0,
       168,  0,  0,
       167,  0,  0,
       166,  0,  0,
       165,  0,  0,
       164,  0,  0,
       163,  0,  0,
       162,  0,  0,
       161,  0,  0,
       160,  0,  0,
       159,  0,  0,
       158,  0,  0,
       157,  0,  0,
       156,  0,  0,
       155,  0,  0,
       154,  0,  0,
       153,  0,  0,
       152,  0,  0,
       151,  0,  0,
       150,  0,  0,
       149,  0,  0,
       148,  0,  0,
       147,  0,  0,
       146,  0,  0,
       145,  0,  0,
       144,  0,  0,
       143,  0,  0,
       142,  0,  0,
       141,  0,  0,
       140,  0,  0,
       139,  0,  0,
       138,  0,  0,
       137,  0,  0,
       136,  0,  0,
       135,  0,  0,
       134,  0,  0,
       133,  0,  0,
       132,  0,  0,
       131,  0,  0,
       130,  0,  0,
       129,  0,  0,
       128,  0,  0};



#endif // STATISTICSDIALOG_H
