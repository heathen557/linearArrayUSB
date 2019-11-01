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


//    //暂时屏蔽 单位换算未最终裁定
//    int x = event->x();
//    int y = event->y();
//    float origin_x  = helper.Window_wid/2.0;
//    float origin_y = helper.Window_height;
//    float distance = sqrtf(pow((origin_x-x),2)+pow(origin_y-y,2));    //计算鼠标点击处的坐标到原点的距离值,这个值没有经过换算
//    QString str;

//    float lestAngle = 90 - helper.showAngle/2.0;

//    float maxDistance = origin_x/cos(lestAngle*3.1415926/180.0);
//    qDebug()<<" maxLength = "<<maxDistance<<endl;


////    /********、此处涉及到一些坐标的变换，tof(0~1333.3),转换比例    *********/
////    // maxDistance  (maxTOF，暂时LSB=1.5)             distance(inputTOF)
////    //----------------------------------  =    ---------------------------------
////    //        width/(2*cos30)                       newDistance(newDistance(axis) )

//    float offset;

//    //在左侧的时候，上下都是负的，得到的结果是正的；    在右侧的时候横坐标是正的，纵坐标是负的
//    if(x != origin_x)
//        offset = (y - origin_y)/(x - origin_x);
//    float angle = atan(offset)*180/3.14159;

//    qDebug()<<"  the angle  =  "<<angle<<endl;

//    if(angle>=lestAngle || angle<=-lestAngle)
//    {
//        int showTOF = (helper.maxDistance_ * distance * cos(lestAngle*3.14159/180.0))/origin_x;
//        str.append("x=").append(QString::number(x)).append(",y=").append(QString::number(y)).append(",tof=").append(QString::number(showTOF));
//        QToolTip::showText(QCursor::pos(),str);
//    }
}


double GLWidget::Dr_calculate(float disTance,float lValue,float theta)
{
    double dr = ((disTance*disTance)-(lValue*lValue))/(2*(disTance+lValue*sin(theta)));
    return dr;
}
