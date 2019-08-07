#include "dealusb_msg.h"
#include<qdebug.h>





//extern bool isWriteSuccess;    //写入命令是否成功标识



/*保存用到的标识*/
//extern bool isSaveFlag;        //是否进行存储
extern QString saveFilePath;   //保存的路径  E:/..../.../的形式
extern int saveFileIndex;      //文件标号；1作为开始


DealUsb_msg::DealUsb_msg(QObject *parent) : QObject(parent)
{

     qDebug()<<"DealUsb_msg thread start!!!!"<<endl;
    isFirstLink = true;

}

void DealUsb_msg::recvMsgSlot(QByteArray array)
{
    int ret;
    char *MyBuffer;

    MyBuffer = array.data();

//    isWriteSuccess = true;    //写入命令是否成功标识，成功以后才能点击“显示”按钮操作
    int imgRow,imgCol;
    int spadNum = MyBuffer[0] + (((ushort)MyBuffer[1]) << 8);
    int line_number = MyBuffer[2] + (((ushort)MyBuffer[3]) << 8);
//    qDebug()<<"here   spadNum = "<<spadNum<<"  line_number = "<<line_number<<endl;


    if(spadNum==0 && lastSpadNum==7)  //此时说明上一帧数据已经接收完毕，把整帧数据付给其他线程，供其显示，数据可以显示了
    {

        emit staticValueSignal(tofMin,tofMax,peakMin,peakMax,xMin,xMax,yMin,yMax,zMin,zMax);
        //重置变量
        tofMin = 10000;
        tofMax = -10000;
        peakMin = 10000;
        peakMax = -10000;
        xMin = 10000;
        xMax = -10000;
        yMin = 10000;
        yMax = -10000;
        zMin = 10000;
        zMax = -10000;
    }

    int line_offset = spadNum / 2;
    int row_offset = (spadNum + 1) % 2;      //表示是在第一行 还是在第二行

    for(int i=0; i<64; i++)
    {

        int tof = (ushort)((MyBuffer[4 + i * 4]) + (((ushort)MyBuffer[4 + i * 4 + 1]) << 8));
        int intensity = (ushort)((MyBuffer[4 + i * 4 + 2]) + (((ushort)MyBuffer[4 + i * 4 + 3]) << 8));


        //行列以及颜色传递给图像
        imgRow = i * 4 + line_offset;
        imgCol = line_number * 2 + row_offset;

//        qDebug()<<"index =="<< imgCol*256+imgRow<<endl;
        cloudIndex = imgCol*256+imgRow;      //在点云数据中的标号

        if(imgRow>=0 && imgRow<256 && imgCol>=0 && imgCol<64)
        {
          //统计二维图像
            tofMax = (tof>tofMax) ? tof : tofMax;
            tofMin = (tof<tofMin) ? tof : tofMin;
            peakMax = (intensity>peakMax) ? intensity : peakMax;
            peakMin = (intensity<peakMin) ? intensity : peakMin;
        }
        else
            qDebug()<<"给像素赋值时出现异常 imgrow="<<imgRow<<"   imgCol = "<<imgCol<<endl;
    }
    lastSpadNum = spadNum ;



}
