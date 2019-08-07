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
