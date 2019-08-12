#include "dealusb_msg.h"
#include<qdebug.h>
#include<vector>
#include<QMutex>

/*保存用到的标识*/
extern QString saveFilePath;   //保存的路径  E:/..../.../的形式
extern int saveFileIndex;      //文件标号；1作为开始
extern bool isSaveFlag;
extern vector<vector<int>> AllPoint_vec;    //保存要显示的点
extern QMutex m_mutex;

extern int showFrameNum;   //同时显示多少帧数据
extern int showTOFmax;     //设置显示的最大范围（m）


DealUsb_msg::DealUsb_msg(QObject *parent) : QObject(parent)
{
     qDebug()<<"DealUsb_msg thread start!!!!"<<endl;
}

void DealUsb_msg::recvMsgSlot(QByteArray array)
{
    char *MyBuffer;
    MyBuffer = array.data();

    //spadNum ==08  lineNum == 00 01 02 03
    int spadNum = MyBuffer[0] + (((ushort)MyBuffer[1]) << 8);
    int line_number = MyBuffer[2] + (((ushort)MyBuffer[3]) << 8);
//    qDebug()<<"here   spadNum = "<<spadNum<<"  line_number = "<<line_number<<endl;

    if(spadNum != 8)          //固定值0x08
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
            emit saveTXTSignal(tofPeakToSave_string);
            tofPeakToSave_string.clear();
        }

        //显示内容相关，将一帧数据传递给全局变量供显示
        if(!Rece_points.empty())
        {
//            qDebug()<<"AllPoint_vec already have number,Rece_points ="<<Rece_points.size()<<",   AllPoint_vec.size="<<AllPoint_vec.size()<<endl;
            m_mutex.lock();
            AllPoint_vec.push_back(Rece_points);
            Rece_points.clear();

            if(AllPoint_vec.size() == showFrameNum+1)  //循环清理第一个元素,因为每次只显示一帧数据，故这里把容器的长度设置为2
            {
                AllPoint_vec.erase(AllPoint_vec.begin(),AllPoint_vec.begin()+1);

            }
            if(AllPoint_vec.size() > showFrameNum+1)
            {
                AllPoint_vec.clear();
            }

            m_mutex.unlock();
        }


    }

    for(int i=0; i<64; i++)                     //260个字节，2个字节spad,2个字节的line,256个字节的数据，一个点由两个字节tof，两个子节点peak构成
    {
        int tof = (ushort)((MyBuffer[4 + i * 4]) + (((ushort)MyBuffer[4 + i * 4 + 1]) << 8));
        int intensity = (ushort)((MyBuffer[4 + i * 4 + 2]) + (((ushort)MyBuffer[4 + i * 4 + 3]) << 8));

        //保存文件时，tofPeakToSave_string存储相关的信息
        if(isSaveFlag)     //如果需要保存文件信息
        {
            tofPeakToSave_string.append(QString::number(tof)).append(",").append(QString::number(intensity)).append("\n");
        }


        //第一个存角度，第二个存tof
        //解析角度值，并保存在vector容器当中;
        if(line_number == 0 || line_number == 1)
        {
            pointIndex = i + 64*line_number;
            angle = -60 + pointIndex*(60.0/128.0);

        }else if(line_number == 2 || line_number == 3)
        {
            pointIndex = i + 64*(line_number-2);
            angle = pointIndex * (60.0/128.0);
        }
        Rece_points.push_back(angle);
        Rece_points.push_back(tof);

//        qDebug()<<"angle = "<<angle<<"   tof="<<tof<<endl;


        //统计tof 以及peak信息
        tofMax = (tof>tofMax) ? tof : tofMax;
        tofMin = (tof<tofMin) ? tof : tofMin;
        peakMax = (intensity>peakMax) ? intensity : peakMax;
        peakMin = (intensity<peakMin) ? intensity : peakMin;

    }
    lastLineNum = line_number ;
}
