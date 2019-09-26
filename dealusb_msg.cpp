#include "dealusb_msg.h"
#include<qdebug.h>
#include<vector>
#include<QMutex>
#include<QFile>

/*保存用到的标识*/
extern QString saveFilePath;   //保存的路径  E:/..../.../的形式
extern int saveFileIndex;      //文件标号；1作为开始
extern bool isSaveFlag;
extern vector<vector<float>> AllPoint_vec;    //保存要显示的点
extern QMutex m_mutex;

//extern int showFrameNum;   //同时显示多少帧数据
//extern int showTOFmax;     //设置显示的最大范围（m）

//统计 均值和方差 相关
extern QMutex statisticMutex;
extern vector<vector<int>> allStatisticTofPoints;   //用于统计 均值和方差的 容器
extern vector<vector<int>> allStatisticPeakPoints;   //用于统计 均值和方差的 容器

DealUsb_msg::DealUsb_msg(QObject *parent) : QObject(parent)
{
     qDebug()<<"DealUsb_msg thread start!!!!"<<endl;

//     showFrameNum = 1;
//     showAngle = 120;
//     showTOFmax = 10;    //10m
     QFile file("setting.ini");
     QByteArray temp("\r\n");
     QString line[20];

     if (file.open(QIODevice::ReadOnly))
     {
         QTextStream in(&file);
         int i = 0;
         while (!in.atEnd())
         {
             line[i] = in.readLine();
             i++;
         }
         file.close();
     }
//     int numSeri_ = line[0].toInt();
//     int baudRateBox_ = line[1].toInt();
     showFrameNum = line[2].toInt();
     showAngle = line[3].toInt();
     showTOFmax = line[4].toInt();







     isTOF_flag = false;
     isShowImageFlag = false; //初始化不发送显示图像的数据


     //总共有256个点 ,针对每一个点开启一个独立的容器进行存储相关内容
     statisticStartFlag = true;    //初始化进行信息统计,后期可尝试改用信号与槽的进制进行传输
     statisticFrameNumber = 10;
     vector<int> singlePoint;
     for(int i=0; i<256; i++)
     {
         tempStatisticTofPoints.push_back(singlePoint);
         tempStatisticPeakPoints.push_back(singlePoint);
         allStatisticTofPoints.push_back(singlePoint);
         allStatisticPeakPoints.push_back(singlePoint);
     }


}

void DealUsb_msg:: showSettingParaSlot(int frameNum, int Angle,int TOFmax)
{
    showFrameNum = frameNum ;
    showAngle = Angle;
    showTOFmax = TOFmax;

    qDebug()<<" DealUsb_msg showSettingParaSlot has coming= "<<frameNum<<"  "<<Angle<<"  "<<TOFmax<<endl;
}

void DealUsb_msg::changeTofPeak_slot()
{
    if(true == isTOF_flag)
    {
        isTOF_flag = false;
    }else
    {
        isTOF_flag = true;
    }
}


//此次协议解析是按照顺序存储的，暂时屏蔽
//void DealUsb_msg::recvMsgSlot(QByteArray array)
//{
//    char *MyBuffer;
//    MyBuffer = array.data();

//    //spadNum ==08  lineNum == 00 01 02 03
//    int spadNum = (quint8)(MyBuffer[0]) +  (((quint8)(MyBuffer[1]))<<8);
//    int line_number = (quint8)(MyBuffer[2]) +  (((quint8)(MyBuffer[3]))<<8);
////    qDebug()<<"here   spadNum = "<<spadNum<<"  line_number = "<<line_number<<endl;

////    if(spadNum != 8)          //固定值0x08
////        return;

//    //如果用面阵数据来进行测试的话 关闭spadNum，打开此行代码
//    if(line_number>3)
//        return;


//    if( line_number ==0 && lastLineNum==3)  //此时说明上一帧数据已经接收完毕，把整帧数据付给其他线程，供其显示，数据可以显示了
//    {

//        //统计信息相关内容
//        emit statisticsValueSignal(tofMin,tofMax,peakMin,peakMax);
//        tofMin = 10000;     //重置变量
//        tofMax = -10000;
//        peakMin = 10000;
//        peakMax = -10000;

//        //如果选中保存，把上一帧的数据发送到数据保存线程中，保存成文本
//        if(isSaveFlag)
//        {
//            emit saveTXTSignal(tofPeakToSave_string);
//            tofPeakToSave_string.clear();
//        }



//        //显示内容相关，将一帧数据传递给全局变量供显示
//        if(!Rece_points.empty())
//        {
////            qDebug()<<"AllPoint_vec already have number,Rece_points ="<<Rece_points.size()<<",   AllPoint_vec.size="<<AllPoint_vec.size()<<endl;
//            m_mutex.lock();
//            AllPoint_vec.push_back(Rece_points);
//            Rece_points.clear();

//            if(AllPoint_vec.size() == showFrameNum+1)  //循环清理第一个元素,因为每次只显示一帧数据，故这里把容器的长度设置为2,这里是用来显示的容器
//            {
//                AllPoint_vec.erase(AllPoint_vec.begin(),AllPoint_vec.begin()+1);

//            }
//            if(AllPoint_vec.size() > showFrameNum+1)
//            {
//                AllPoint_vec.clear();
//            }

//            m_mutex.unlock();
//        }


//        //统计信息相关的 ，将统计信息的容器赋值给全局变量
//        if(statisticStartFlag)
//        {
//            statisticMutex.lock();
//            allStatisticTofPoints = tempStatisticTofPoints;
//            allStatisticPeakPoints = tempStatisticPeakPoints;
//            statisticMutex.unlock();
//        }
//    }    //一帧已经接收完毕



//    for(int i=0; i<64; i++)                     //260个字节，2个字节spad,2个字节的line,256个字节的数据，一个点由两个字节tof，两个子节点peak构成  一个包有64个点
//    {

//        int tof,intensity;
//        if(isTOF_flag)
//        {
//            tof = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
//            intensity = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
//        }else
//        {
//            intensity = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
//            tof = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
//        }


////        qDebug()<<"tof = "<<tof<<" intensity ="<<intensity<<endl;

//        //保存文件时，tofPeakToSave_string存储相关的信息
//        if(isSaveFlag)     //如果需要保存文件信息
//        {
//            tofPeakToSave_string.append(QString::number(tof)).append(",").append(QString::number(intensity)).append("\n");
//        }

//        /********************************************************************************************************************************/
//        //开始存储统计信息；
//        if(statisticStartFlag == true)
//        {
//            statisticIndex = i + 64*line_number;    // statisticIndex = line_number + 4*i ;

//            //判断每个点已经储存的个数，如果已经超过设定的范围，则进行循环储存；
//            int offset = tempStatisticTofPoints[statisticIndex].size() - statisticFrameNumber;
//            if(offset >= 0)
//            {
//                tempStatisticTofPoints[statisticIndex].erase(tempStatisticTofPoints[statisticIndex].begin(),tempStatisticTofPoints[statisticIndex].begin()+offset+1);
//                tempStatisticPeakPoints[statisticIndex].erase(tempStatisticPeakPoints[statisticIndex].begin(),tempStatisticPeakPoints[statisticIndex].begin()+offset+1);
//            }

//            //向每个点的容器中添加一个新的点,完成循环存储
//            tempStatisticTofPoints[statisticIndex].push_back(tof);
//            tempStatisticPeakPoints[statisticIndex].push_back(intensity);
//        }
//        /*******************************************************************************************************************************/


//        //第一个存角度，第二个存tof
//        //解析角度值，并保存在vector容器当中;
//        if(line_number == 0 || line_number == 1)
//        {
//            pointIndex = i + 64*line_number;
//            angle = -showAngle/2.0 + pointIndex*((showAngle/2.0)/128.0);

//        }else if(line_number == 2 || line_number == 3)
//        {
//            pointIndex = i + 64*(line_number-2);
//            angle = pointIndex * ((showAngle/2.0)/128.0);
//        }

//        Rece_points.push_back(angle);
//        Rece_points.push_back(tof);

////        qDebug()<<"angle = "<<angle<<"   tof="<<tof<<endl;

//        //统计tof 以及peak信息
//        tofMax = (tof>tofMax) ? tof : tofMax;
//        tofMin = (tof<tofMin) ? tof : tofMin;
//        peakMax = (intensity>peakMax) ? intensity : peakMax;
//        peakMin = (intensity<peakMin) ? intensity : peakMin;

//    }
//    lastLineNum = line_number ;
//}





//协议版本为spad==09 ，  1*256协议 ；
void DealUsb_msg::recvMsgSlot(QByteArray array)
{
    char *MyBuffer;
    MyBuffer = array.data();

    //spadNum ==08  lineNum == 00 01 02 03
    int spadNum = (quint8)(MyBuffer[0]) +  (((quint8)(MyBuffer[1]))<<8);
    int line_number = (quint8)(MyBuffer[2]) +  (((quint8)(MyBuffer[3]))<<8);
//    qDebug()<<"here   spadNum = "<<spadNum<<"  line_number = "<<line_number<<endl;

    if(spadNum != 9)          //固定值0x08
        return;

    //如果用面阵数据来进行测试的话 关闭spadNum，打开此行代码
    if(line_number>3)
        return;


    if( line_number ==0 && lastLineNum==3)  //此时说明上一帧数据已经接收完毕，把整帧数据付给其他线程，供其显示，数据可以显示了
    {

        //统计信息相关内容
        emit statisticsValueSignal(tofMin,tofMax,peakMin,peakMax);
        tofMin = 10000;     //重置变量
        tofMax = -10000;
        peakMin = 10000;
        peakMax = -10000;

        //如果选中保存，把上一帧的数据发送到数据保存线程中，保存成文本
        if(isSaveFlag)
        {
            for(int i=0; i<256; i++)
            {
                tofPeakToSave_string.append(tmpTofPeak_string[i]);
            }
            emit saveTXTSignal(tofPeakToSave_string);
            tofPeakToSave_string.clear();
        }



        //显示数据发送给接收容器
        //256个点 分为左右两个
        //   另将256个tof以及PEAK存储在QStringList当中，以信号的形式发送给统计界面以供显示
        int i = 0;
        int tmpTof,tmpPeak;
        for(i=0; i<128; i++)   //存储前128个点
        {
            int leftIndex = i;
            angle = -showAngle/2.0 + leftIndex*((showAngle/2.0)/128.0);
            tmpTof = imageArray[0][i];
            Rece_points.push_back(angle);
            Rece_points.push_back(tmpTof);

            /**tof 、peak图像存储数据**/
            tmpPeak = imageArray_peak[0][i];
            tofList.append(QString::number(tmpTof));
            peakList.append(QString::number(tmpPeak));
        }

        for(i=128; i<256; i++)
        {
            int rightIndex = i-128;
            angle = rightIndex * ((showAngle/2.0)/128.0);
            tmpTof = imageArray[0][i];
            Rece_points.push_back(angle);
            Rece_points.push_back(tmpTof);

            /**tof 、peak图像存储数据**/
            tmpPeak = imageArray_peak[0][i];
            tofList.append(QString::number(tmpTof));
            peakList.append(QString::number(tmpPeak));
        }

        //将tof和peak的统计值发送给统计界面以供来显示,并清空字符串连
        if(true == isShowImageFlag)
        {
            emit tofPeakImageSignal(tofList,peakList,1);
        }
        tofList.clear();
        peakList.clear();

        //显示内容相关，将一帧数据传递给全局变量供显示
        if(!Rece_points.empty())
        {
//            qDebug()<<"AllPoint_vec already have number,Rece_points ="<<Rece_points.size()<<",   AllPoint_vec.size="<<AllPoint_vec.size()<<endl;
            m_mutex.lock();
            AllPoint_vec.push_back(Rece_points);
            Rece_points.clear();

            if(AllPoint_vec.size() == showFrameNum+1)  //循环清理第一个元素,因为每次只显示一帧数据，故这里把容器的长度设置为2,这里是用来显示的容器
            {
                AllPoint_vec.erase(AllPoint_vec.begin(),AllPoint_vec.begin()+1);

            }
            if(AllPoint_vec.size() > showFrameNum+1)
            {
                AllPoint_vec.clear();
            }

            m_mutex.unlock();
        }


        //统计信息相关的 ，将统计信息的容器赋值给全局变量
        if(statisticStartFlag)
        {
            statisticMutex.lock();
            allStatisticTofPoints = tempStatisticTofPoints;
            allStatisticPeakPoints = tempStatisticPeakPoints;
            statisticMutex.unlock();
        }
    }    //一帧已经接收完毕



    for(int i=0; i<64; i++)                     //260个字节，2个字节spad,2个字节的line,256个字节的数据，一个点由两个字节tof，两个子节点peak构成  一个包有64个点
    {

        int tof,intensity;
        if(isTOF_flag)                          //TOF和PEAk 存在前后不确定的问题，故设定标识位进行选择
        {
            tof = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
            intensity = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
        }else
        {
            intensity = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
            tof = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
        }


//        qDebug()<<"tof = "<<tof<<" intensity ="<<intensity<<endl;

        pointIndex = line_number + 4 * i;        //pointIndex 为256个点的序号
                                                 //为最新的协议lineNum=0: 0 4 8 12...
                                                 //          lineNum=1: 1 5 9 13...
                                                 //          lineNum=2: 2 6 10 14...
                                                 //          lineNum=3: 3 7 11 15 ...

        //保存文件时，tofPeakToSave_string存储相关的信息
        if(isSaveFlag)     //如果需要保存文件信息
        {
//            tofPeakToSave_string.append(QString::number(tof)).append(",").append(QString::number(intensity)).append("\n");

            tmpTofPeak_string[pointIndex] = QString::number(tof).append(",").append(QString::number(intensity)).append("\n");
        }

        /********************************************************************************************************************************/
        //开始存储统计信息；
        if(statisticStartFlag == true)
        {
            statisticIndex = pointIndex;    // statisticIndex = line_number + 4*i ;

            //判断每个点已经储存的个数，如果已经超过设定的范围，则进行循环储存；
            int offset = tempStatisticTofPoints[statisticIndex].size() - statisticFrameNumber;
            if(offset >= 0)
            {
                tempStatisticTofPoints[statisticIndex].erase(tempStatisticTofPoints[statisticIndex].begin(),tempStatisticTofPoints[statisticIndex].begin()+offset+1);
                tempStatisticPeakPoints[statisticIndex].erase(tempStatisticPeakPoints[statisticIndex].begin(),tempStatisticPeakPoints[statisticIndex].begin()+offset+1);
            }

            //向每个点的容器中添加一个新的点,完成循环存储
            tempStatisticTofPoints[statisticIndex].push_back(tof);
            tempStatisticPeakPoints[statisticIndex].push_back(intensity);
        }
        /*******************************************************************************************************************************/

        if( pointIndex<256 )
        {
            imageArray[0][pointIndex] = tof;
            imageArray_peak[0][pointIndex] = intensity;
        }else
        {
            qDebug()<<QStringLiteral("给像素赋值时出现异常 pointIndex=")<<pointIndex<<endl;
        }



//        qDebug()<<"angle = "<<angle<<"   tof="<<tof<<endl;

        //统计tof 以及peak信息
        tofMax = (tof>tofMax) ? tof : tofMax;
        tofMin = (tof<tofMin) ? tof : tofMin;
        peakMax = (intensity>peakMax) ? intensity : peakMax;
        peakMin = (intensity<peakMin) ? intensity : peakMin;

    }
    lastLineNum = line_number ;
}



//2*256的协议解析  spad:0-7  lineNum:0
void DealUsb_msg::recvMsgSlot_2_256(QByteArray array)
{

    char *MyBuffer;
    MyBuffer = array.data();

    //spadNum ==00 -- 07   lineNum == 00
    int spadNum = (quint8)(MyBuffer[0]) +  (((quint8)(MyBuffer[1]))<<8);
    int line_number = (quint8)(MyBuffer[2]) +  (((quint8)(MyBuffer[3]))<<8);
//    qDebug()<<"recvMsgSlot_2_256   spadNum = "<<spadNum<<"  line_number = "<<line_number<<endl;

//    if(spadNum != 8)          //固定值0x08
//        return;

    //如果用面阵数据来进行测试的话 关闭spadNum，打开此行代码
    if(line_number != 0)
    {
        qDebug()<<QStringLiteral("line_number不为0 则返回了")<<endl;
         return;
    }

    //一帧已经接收完成
    if(spadNum == 0 && lastSpadNum == 7 )
    {
        //统计信息相关内容
        emit statisticsValueSignal(tofMin,tofMax,peakMin,peakMax);
        tofMin = 10000;     //重置变量
        tofMax = -10000;
        peakMin = 10000;
        peakMax = -10000;



        //显示数据发送给接收容器
        //256个点 分为左右两个
        int i = 0;
        int tmpTof, tmpPeak;
        for(i=0; i<128; i++)   //存储前128个点
        {
            int leftIndex = i;
            angle = -showAngle/2.0 + leftIndex*((showAngle/2.0)/128.0);
//            tmpTof = imageArray[0][i];
            tmpTof = (imageArray[0][i] + imageArray[1][i])/2.0;

            Rece_points.push_back(angle);
            Rece_points.push_back(tmpTof);
        }

        for(i=128; i<256; i++)
        {
            int rightIndex = i-128;
            angle = rightIndex * ((showAngle/2.0)/128.0);
//            tmpTof = imageArray[0][i];
            tmpTof = (imageArray[0][i] + imageArray[1][i])/2.0;
            Rece_points.push_back(angle);
            Rece_points.push_back(tmpTof);
        }


        //将tof和peak的统计值发送给统计界面以供来显示,并清空字符串连
        if(true == isShowImageFlag)
        {
            int rowNum=0,colNum;
            for(i=0;i<512;i++)
            {
                colNum = i;
                if(i>255)
                {
                    rowNum = 1;
                    colNum = i-256;
                }
                tmpTof = imageArray[rowNum][colNum];
                tmpPeak = imageArray_peak[rowNum][colNum];
                tofList.append(QString::number(tmpTof));
                peakList.append(QString::number(tmpPeak));
            }

            emit tofPeakImageSignal(tofList,peakList,2);    //两行
        }
        tofList.clear();
        peakList.clear();




        //显示内容相关，将一帧数据传递给全局变量供显示
        if(!Rece_points.empty())
        {
//            qDebug()<<"AllPoint_vec already have number,Rece_points ="<<Rece_points.size()<<",   AllPoint_vec.size="<<AllPoint_vec.size()<<endl;
            m_mutex.lock();
            AllPoint_vec.push_back(Rece_points);
            Rece_points.clear();

            if(AllPoint_vec.size() == showFrameNum+1)  //循环清理第一个元素,因为每次只显示一帧数据，故这里把容器的长度设置为2,这里是用来显示的容器
            {
                AllPoint_vec.erase(AllPoint_vec.begin(),AllPoint_vec.begin()+1);

            }
            if(AllPoint_vec.size() > showFrameNum+1)
            {
                AllPoint_vec.clear();
            }

//            qDebug()<<" allPoint_vec.size() = "<<AllPoint_vec.size()<<endl;

            m_mutex.unlock();
        }

        /************************************************************************************************/





        //如果选中保存，把上一帧的数据发送到数据保存线程中，保存成文本
        if(isSaveFlag)
        {
            for(int i=0; i<512; i++)
            {
                tofPeakToSave_string.append(tmpTofPeak_string[i]);
            }

            emit saveTXTSignal(tofPeakToSave_string);
            tofPeakToSave_string.clear();
        }




        //统计信息相关 统计信息容器赋值给全局变量
        if(statisticStartFlag)
        {
            statisticMutex.lock();
            allStatisticTofPoints = tempStatisticTofPoints;
            allStatisticPeakPoints = tempStatisticPeakPoints;
            statisticMutex.unlock();
        }
        //向主线程中发送最大值、最小值等统计信息

    } //以上为处理完整的一帧数据



    //********************开始对单包数据进行解析*****************

    int line_offset = spadNum/2;
    int row_offset = (spadNum + 1)%2;
    for(int i=0; i<64; i++)
    {
        int tof,intensity;
        if(isTOF_flag )  //基本上都是反的 所以直接设置为不可能的值
        {
            tof = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
            intensity = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
        }else
        {
            intensity = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
            tof = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
        }


        //行列以及TOF值信息传递给图像
        int imgRow = i * 4 + line_offset;
        int imgCol = line_number * 2 + row_offset;
        int cloudIndex = imgCol*256+imgRow;      //在点云数据中的标号

        if(imgRow>=0 && imgRow<256 && imgCol>=0 && imgCol<2)
        {
            imageArray[imgCol][imgRow] = tof;
        }else
        {
            qDebug()<<QStringLiteral("给像素赋值时出现异常 imgrow=")<<imgRow<<"   imgCol = "<<imgCol<<endl;
        }

        //保存文件时，tofPeakToSave_string存储相关的信息
        if(isSaveFlag)     //如果需要保存文件信息
        {
//            tofPeakToSave_string.append(QString::number(tof)).append(",").append(QString::number(intensity)).append("\n");

            tmpTofPeak_string[cloudIndex] = QString::number(tof).append(",").append(QString::number(intensity)).append("\n");
        }


        /********************************************************************************************************************************/
        //开始存储统计信息；
        if(statisticStartFlag == true)
        {
            if(cloudIndex<256)
            {
                statisticIndex = cloudIndex;    // statisticIndex = line_number + 4*i ;

                //判断每个点已经储存的个数，如果已经超过设定的范围，则进行循环储存；
                int offset = tempStatisticTofPoints[statisticIndex].size() - statisticFrameNumber;
                if(offset >= 0)
                {
                    tempStatisticTofPoints[statisticIndex].erase(tempStatisticTofPoints[statisticIndex].begin(),tempStatisticTofPoints[statisticIndex].begin()+offset+1);
                    tempStatisticPeakPoints[statisticIndex].erase(tempStatisticPeakPoints[statisticIndex].begin(),tempStatisticPeakPoints[statisticIndex].begin()+offset+1);
                }

                //向每个点的容器中添加一个新的点,完成循环存储
                tempStatisticTofPoints[statisticIndex].push_back(tof);
                tempStatisticPeakPoints[statisticIndex].push_back(intensity);
            }

        }
        /*******************************************************************************************************************************/




        //统计tof 以及peak信息
        tofMax = (tof>tofMax) ? tof : tofMax;
        tofMin = (tof<tofMin) ? tof : tofMin;
        peakMax = (intensity>peakMax) ? intensity : peakMax;
        peakMin = (intensity<peakMin) ? intensity : peakMin;


    } //64个点循环

    lastSpadNum = spadNum;
}
/************************以上是 2*256 版本协议的解析 **************************************************/




/************************以下是 4*256 版本协议的解析***************************************************/
//@brief : 协议spadNum:00-07      lineNum:0 1
void DealUsb_msg::recvMsgSlot_4_256(QByteArray array)
{
    char *MyBuffer;
    MyBuffer = array.data();
//    qDebug()<<"recvMsgSlot_4_256 "<<endl;

    //spadNum ==00 -- 07   lineNum == 00
    int spadNum = (quint8)(MyBuffer[0]) +  (((quint8)(MyBuffer[1]))<<8);
    int line_number = (quint8)(MyBuffer[2]) +  (((quint8)(MyBuffer[3]))<<8);
    qDebug()<<"recvMsgSlot_4_256   spadNum = "<<spadNum<<"  line_number = "<<line_number<<endl;

//    if(spadNum != 8)          //固定值0x08
//        return;

    //如果用面阵数据来进行测试的话 关闭spadNum，打开此行代码
    if(line_number >1)
    {
         qDebug()<<QStringLiteral(" line_number>1 ,数据不符合要求 ")<<endl;
         return;
    }

    //一帧已经接收完成
    if(spadNum == 0 && lastSpadNum == 7 )
    {
        //统计信息相关内容
        emit statisticsValueSignal(tofMin,tofMax,peakMin,peakMax);
        tofMin = 10000;     //重置变量
        tofMax = -10000;
        peakMin = 10000;
        peakMax = -10000;



        //显示数据发送给接收容器
        //256个点 分为左右两个
        int i = 0;
        int tmpTof,tmpPeak;
        for(i=0; i<128; i++)   //存储前128个点
        {
            int leftIndex = i;
            angle = -showAngle/2.0 + leftIndex*((showAngle/2.0)/128.0);
//            tmpTof = imageArray[2][i];
            tmpTof = (imageArray[2][i] + imageArray[3][i] )/2.0;
            Rece_points.push_back(angle);
            Rece_points.push_back(tmpTof);
        }

        for(i=128; i<256; i++)
        {
            int rightIndex = i-128;
            angle = rightIndex * ((showAngle/2.0)/128.0);
//            tmpTof = imageArray[2][i];
            tmpTof = (imageArray[2][i] + imageArray[3][i])/2.0;
            Rece_points.push_back(angle);
            Rece_points.push_back(tmpTof);
        }


        //将tof和peak的统计值发送给统计界面以供来显示,并清空字符串连
        if(true == isShowImageFlag)
        {
            int rowNum=0,colNum;
            for(i=0;i<1024;i++)
            {
                if(i<256)
                {
                    rowNum = 0;
                    colNum = i;
                }else if(i<512)
                {
                    rowNum = 1;
                    colNum = i - 256;
                }else if(i<768)
                {
                    rowNum = 2;
                    colNum = i - 512;
                }else if(i<1024)
                {
                    rowNum = 3;
                    colNum = i - 768;
                }
                tmpTof = imageArray[rowNum][colNum];
                tmpPeak = imageArray_peak[rowNum][colNum];
                tofList.append(QString::number(tmpTof));
                peakList.append(QString::number(tmpPeak));
            }

            emit tofPeakImageSignal(tofList,peakList,4);    //四行
        }
        tofList.clear();
        peakList.clear();



        //显示内容相关，将一帧数据传递给全局变量供显示
        if(!Rece_points.empty())
        {
//            qDebug()<<"AllPoint_vec already have number,Rece_points ="<<Rece_points.size()<<",   AllPoint_vec.size="<<AllPoint_vec.size()<<endl;
            m_mutex.lock();
            AllPoint_vec.push_back(Rece_points);
            Rece_points.clear();

            if(AllPoint_vec.size() == showFrameNum+1)  //循环清理第一个元素,因为每次只显示一帧数据，故这里把容器的长度设置为2,这里是用来显示的容器
            {
                AllPoint_vec.erase(AllPoint_vec.begin(),AllPoint_vec.begin()+1);

            }
            if(AllPoint_vec.size() > showFrameNum+1)
            {
                AllPoint_vec.clear();
            }

            m_mutex.unlock();
        }

        /************************************************************************************************/





        //如果选中保存，把上一帧的数据发送到数据保存线程中，保存成文本
        if(isSaveFlag)
        {
            for(int i=0; i<(256*4); i++)
            {
                tofPeakToSave_string.append(tmpTofPeak_string[i]);
            }

            emit saveTXTSignal(tofPeakToSave_string);
            tofPeakToSave_string.clear();
        }




        //统计信息相关 统计信息容器赋值给全局变量
        if(statisticStartFlag)
        {
            statisticMutex.lock();
            allStatisticTofPoints = tempStatisticTofPoints;
            allStatisticPeakPoints = tempStatisticPeakPoints;
            statisticMutex.unlock();
        }
        //向主线程中发送最大值、最小值等统计信息

    } //以上为处理完整的一帧数据



    //********************开始对单包数据进行解析*****************

    int line_offset = spadNum/2;         // 0 1 2 3
    int row_offset = (spadNum + 1)%2;    //0 1
    for(int i=0; i<64; i++)
    {
        int tof,intensity;
        if(isTOF_flag )    //基本上都是反的 所以直接设置为不可能的值
        {
            tof = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
            intensity = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
        }else
        {
            intensity = quint8(MyBuffer[4 + i * 4]) + ((quint8(MyBuffer[4 + i * 4 +1]))<<8);
            tof = quint8(MyBuffer[4 + i * 4 + 2]) + ((quint8(MyBuffer[4 + i * 4 + 3 ]))<<8);
        }


        //行列以及TOF值信息传递给图像
        int imgRow = i * 4 + line_offset;
        int imgCol = line_number * 2 + row_offset;
        int cloudIndex = imgCol*256+imgRow;      //在点云数据中的标号
//        qDebug()<<"imgRow = "<<imgRow<<" imgCol ="<<imgCol<<endl;

        if(imgRow>=0 && imgRow<256 && imgCol>=0 && imgCol<4)
        {
            imageArray[imgCol][imgRow] = tof;
        }else
        {
            qDebug()<<QStringLiteral("给像素赋值时出现异常 imgrow=")<<imgRow<<"   imgCol = "<<imgCol<<endl;
        }

        //保存文件时，tofPeakToSave_string存储相关的信息
        if(isSaveFlag)     //如果需要保存文件信息
        {
//            tofPeakToSave_string.append(QString::number(tof)).append(",").append(QString::number(intensity)).append("\n");

            tmpTofPeak_string[cloudIndex] = QString::number(tof).append(",").append(QString::number(intensity)).append("\n");
        }



        /********************************************************************************************************************************/
        //开始存储统计信息；
        if(statisticStartFlag == true)
        {
            if(cloudIndex>511  && cloudIndex<768)     //256*2-1 =511     256*3 = 768
            {
                statisticIndex = cloudIndex-512;    // statisticIndex = line_number + 4*i ;

                //判断每个点已经储存的个数，如果已经超过设定的范围，则进行循环储存；
                int offset = tempStatisticTofPoints[statisticIndex].size() - statisticFrameNumber;
                if(offset >= 0)
                {
                    tempStatisticTofPoints[statisticIndex].erase(tempStatisticTofPoints[statisticIndex].begin(),tempStatisticTofPoints[statisticIndex].begin()+offset+1);
                    tempStatisticPeakPoints[statisticIndex].erase(tempStatisticPeakPoints[statisticIndex].begin(),tempStatisticPeakPoints[statisticIndex].begin()+offset+1);
                }

                //向每个点的容器中添加一个新的点,完成循环存储
                tempStatisticTofPoints[statisticIndex].push_back(tof);
                tempStatisticPeakPoints[statisticIndex].push_back(intensity);
            }

        }
        /*******************************************************************************************************************************/



        //统计tof 以及peak信息
        tofMax = (tof>tofMax) ? tof : tofMax;
        tofMin = (tof<tofMin) ? tof : tofMin;
        peakMax = (intensity>peakMax) ? intensity : peakMax;
        peakMin = (intensity<peakMin) ? intensity : peakMin;

    } //64个点循环
    lastSpadNum = spadNum;
}




/***********************************串口的协议解析   4*256******************************************************/
//此处接收到的是一整帧的数据，解析采用郑阳的程序
void DealUsb_msg::recvSerialSlot_4_256(QByteArray MyBuffer)
{
    int cloudIndex ;      //在点云数据中的标号

//    qDebug()<<" myBuffer'size = "<<MyBuffer.size()<<endl;


    for (int spad_no = 0; spad_no < 8; spad_no++)
    {
        int spad_no_heng = spad_no / 2;                     //0 1 2 3
        int spad_no_shu = (spad_no + 1) % 2;				//0 1

        for (int line = 0; line < 2; line++)
        {
            int line_backup = line * 2 + spad_no_shu;      // 0 1 2 3
            for (int row = 0; row < 64; row++)
            {
                int row_backup = row * 4 + spad_no_heng;  // 0 ----- 255

                int tof,intensity;
                if(!isTOF_flag )    //基本上都是反的 所以直接设置为不可能的值
                {
                    tof = quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 0]) + ((quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 1]))<<8);
                    intensity = quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 2]) + ((quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 3]))<<8);
                }else
                {
                    intensity = quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 0]) + ((quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 1]))<<8);
                    tof = quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 2]) + ((quint8(MyBuffer[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 3]))<<8);
                }
//                int data_ls = (para.data[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 0]) + (para.data[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 1] << 8);
//                A4_L_picture[line_backup, row_backup].position = (ushort)data_ls;
//                data_ls = para.data[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 2] + (para.data[spad_no * 64 * 2 * 4 + line * 256 + row * 4 + 3] << 8);
//                A4_L_picture[line_backup, row_backup].counter = (ushort)data_ls;

                imageArray[line_backup][row_backup] = tof;

                //保存文件时，tofPeakToSave_string存储相关的信息
                cloudIndex = line_backup*256 + row_backup;
                if(isSaveFlag)     //如果需要保存文件信息
                {
        //            tofPeakToSave_string.append(QString::number(tof)).append(",").append(QString::number(intensity)).append("\n");
                    tmpTofPeak_string[cloudIndex] = QString::number(tof).append(",").append(QString::number(intensity)).append("\n");
                }


                /********************************************************************************************************************************/
                //开始存储统计信息；
                if(statisticStartFlag == true)
                {
                    if(cloudIndex>511  && cloudIndex<768)     //256*2-1 =511     256*3 = 768
                    {
                        statisticIndex = cloudIndex-512;    // statisticIndex = line_number + 4*i ;

                        //判断每个点已经储存的个数，如果已经超过设定的范围，则进行循环储存；
                        int offset = tempStatisticTofPoints[statisticIndex].size() - statisticFrameNumber;
                        if(offset >= 0)
                        {
                            tempStatisticTofPoints[statisticIndex].erase(tempStatisticTofPoints[statisticIndex].begin(),tempStatisticTofPoints[statisticIndex].begin()+offset+1);
                            tempStatisticPeakPoints[statisticIndex].erase(tempStatisticPeakPoints[statisticIndex].begin(),tempStatisticPeakPoints[statisticIndex].begin()+offset+1);
                        }

                        //向每个点的容器中添加一个新的点,完成循环存储
                        tempStatisticTofPoints[statisticIndex].push_back(tof);
                        tempStatisticPeakPoints[statisticIndex].push_back(intensity);
                    }
                }
                /*******************************************************************************************************************************/

                //统计tof 以及peak信息
                tofMax = (tof>tofMax) ? tof : tofMax;
                tofMin = (tof<tofMin) ? tof : tofMin;
                peakMax = (intensity>peakMax) ? intensity : peakMax;
                peakMin = (intensity<peakMin) ? intensity : peakMin;



            }
        }
    } //spadNo:0->8



    /*******************以上循环以后，4*256个点全部接收完毕 下面对整帧数据进行处理 ********************************************/

    //统计信息相关内容
    emit statisticsValueSignal(tofMin,tofMax,peakMin,peakMax);
    tofMin = 10000;     //重置变量
    tofMax = -10000;
    peakMin = 10000;
    peakMax = -10000;

    //显示数据发送给接收容器
    //256个点 分为左右两个
    int i = 0;
    int tmpTof;
    for(i=0; i<128; i++)   //存储前128个点
    {
        int leftIndex = i;
        angle = -showAngle/2.0 + leftIndex*((showAngle/2.0)/128.0);
//      tmpTof = imageArray[2][i];
        tmpTof = (imageArray[2][i] + imageArray[3][i] )/2.0;
        Rece_points.push_back(angle);
        Rece_points.push_back(tmpTof);
    }

    for(i=128; i<256; i++)
    {
        int rightIndex = i-128;
        angle = rightIndex * ((showAngle/2.0)/128.0);
//      tmpTof = imageArray[2][i];
        tmpTof = (imageArray[2][i] + imageArray[3][i])/2.0;
        Rece_points.push_back(angle);
        Rece_points.push_back(tmpTof);
    }

    //显示内容相关，将一帧数据传递给全局变量供显示
    if(!Rece_points.empty())
    {
//      qDebug()<<"AllPoint_vec already have number,Rece_points ="<<Rece_points.size()<<",   AllPoint_vec.size="<<AllPoint_vec.size()<<endl;
        m_mutex.lock();
        AllPoint_vec.push_back(Rece_points);
        Rece_points.clear();

        if(AllPoint_vec.size() == showFrameNum+1)  //循环清理第一个元素,因为每次只显示一帧数据，故这里把容器的长度设置为2,这里是用来显示的容器
        {
            AllPoint_vec.erase(AllPoint_vec.begin(),AllPoint_vec.begin()+1);

        }
        if(AllPoint_vec.size() > showFrameNum+1)
        {
            AllPoint_vec.clear();
        }

        m_mutex.unlock();
    }

    /***************/

    //如果选中保存，把上一帧的数据发送到数据保存线程中，保存成文本
    if(isSaveFlag)
    {
        for(int i=0; i<(256*4); i++)
        {
            tofPeakToSave_string.append(tmpTofPeak_string[i]);
        }

        emit saveTXTSignal(tofPeakToSave_string);
        tofPeakToSave_string.clear();
    }

    //统计信息相关 统计信息容器赋值给全局变量
    if(statisticStartFlag)
    {
        statisticMutex.lock();
        allStatisticTofPoints = tempStatisticTofPoints;
        allStatisticPeakPoints = tempStatisticPeakPoints;
        statisticMutex.unlock();
    }
    //向主线程中发送最大值、最小值等统计信息

}




void DealUsb_msg::alterStatisticFrameNum_slot(int num,bool isStartFlag)
{
    statisticFrameNumber = num;
    if(true == isStartFlag)
    {
        isShowImageFlag = true;
    }else{
        isShowImageFlag = false;
    }

}
