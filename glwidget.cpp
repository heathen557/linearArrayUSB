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
    int origin_x  = helper.Window_wid/2;
    int origin_y = helper.Window_height;
    float distance = sqrtf(pow((origin_x-x),2)+pow(origin_y-y,2));    //计算鼠标点击处的坐标到原点的距离值,这个值没有经过换算

    qDebug()<<"x = "<<x<<"  y="<<y<<"  window_width"<<helper.Window_wid <<"  window_height="<<helper.Window_height<<"  distance ="<<distance<<endl;
}
