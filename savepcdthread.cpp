#include "savepcdthread.h"
#include<QDebug>
#include<QMutex>
#include<QFile>

QMutex saveMutex;
QString saveTofPeak_string;
extern QString saveFilePath;   //保存的路径  E:/..../.../的形式
extern int saveFileIndex;      //文件标号；1作为开始

savePCDThread::savePCDThread(QObject *parent) : QObject(parent)
{
    qDebug()<<"savePCDThread thread start!!!!"<<endl;
}


void savePCDThread::saveTXTSlot(QString msgStr)
{
    writeTXT(msgStr,saveFileIndex);
    saveFileIndex++;
}



//按照标识写文件
// input:text:写入文本的内容
// numOfFile：第几个文件
void savePCDThread::writeTXT(QString text, int index)
{
    index;
    QString sFilePath = saveFilePath + QString::number(saveFileIndex)+".txt";
    QFile file(sFilePath);
    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);

    QTextStream out(&file);
    out<<text.toLocal8Bit()<<endl;
    file.close();
}
