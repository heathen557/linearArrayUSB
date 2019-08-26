#include "glwidget.h"
#include<QDebug>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

    elapsed = 0;
//    setFixedSize(200, 200);
    setAutoFillBackground(false);
    connect(&timer,SIGNAL(timeout()),this,SLOT(animate()));
//    timer.start(5);



}


void GLWidget::animate()
{
//    helper->Window_wid = this->width();
//    helper->Window_height = this->height();
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
    update();
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    helper.paint(&painter, event, elapsed);
    painter.end();
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    float origin_x  = helper.Window_wid/2.0;
    float origin_y = helper.Window_height;
    float distance = sqrtf(pow((origin_x-x),2)+pow(origin_y-y,2));    //计算鼠标点击处的坐标到原点的距离值,这个值没有经过换算
    QString str;

    float maxDistance = origin_x/cos(30*3.1415926/180.0);
    qDebug()<<" maxLength = "<<maxDistance<<endl;


    /********、此处涉及到一些坐标的变换，tof(0~1333.3),转换比例    *********/
    //           1333.3                 distance(inputTOF)
    //     ------------------  = ---------------------------------
    //      width/(2*cos30)           newDistance(newDistance(axis) )

    float offset;

    //在左侧的时候，上下都是负的，得到的结果是正的；    在右侧的时候横坐标是正的，纵坐标是负的
    if(x != origin_x)
        offset = (y - origin_y)/(x - origin_x);
    float angle = atan(offset)*180/3.14159;

    qDebug()<<"  the angle  =  "<<angle<<endl;

    if(angle>=30 || angle<=-30)
    {
        int showTOF = (helper.maxDistance_ * distance * cos(30*3.14159/180.0))/origin_x;
        str.append("x=").append(QString::number(x)).append(",y=").append(QString::number(y)).append(",tof=").append(QString::number(showTOF));
        QToolTip::showText(QCursor::pos(),str);
    }

//    if(angle >= 30)    //左侧部分的时候显示 x，y,tof
//    {
////        qDebug()<<"x = "<<x<<"  y="<<y<<"  window_width"<<helper.Window_wid <<"  window_height="<<helper.Window_height<<"  distance ="<<distance<<endl;
////        // 接下来就是通过坐标值 反推其 tof的值，然后以供显示用
////        //1、   首先根据y坐标 和角度值推算出newDistance ， float y = window_height - newDistance*cos(ang * PI /180.0)
////        // 所以 newDistance = （window_height - y）/cos(ang * PI/180.0);
////        newDistance = (origin_y-y)/cos((angle-30)*3.14159/180.0);
////        qDebug()<<"newDistance = "<<newDistance<<" (angle-30)"<<angle-30<<endl;

////        // 2 根据公式推算distance
////        int distance = ( helper.maxDistance_*newDistance*cos(30*3.14159/180.0) )/origin_x;


//        int showTOF = (helper.maxDistance_ * distance * cos(30*3.14159/180.0))/origin_x;
//        str.append("x=").append(QString::number(x)).append(",y=").append(QString::number(y)).append(",tof=").append(QString::number(showTOF));
//        QToolTip::showText(QCursor::pos(),str);
//    }

//    if(angle <= -30)   //右侧部分的时候显示 x，y,tof
//    {
//        int showTOF = (helper.maxDistance_ * distance * cos(30*3.14159/180.0))/origin_x;

//        str.append("x=").append(QString::number(x)).append(",y=").append(QString::number(y)).append(",tof=").append(QString::number(showTOF));
//        QToolTip::showText(QCursor::pos(),str);
//        qDebug()<<"x = "<<x<<"  y="<<y<<"  window_width"<<helper.Window_wid <<"  window_height="<<helper.Window_height<<"  distance ="<<distance<<endl;
//    }

}
