#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include<helper.h>
#include <QPainter>
#include <QTimer>
#include<QToolTip>
#include<QMouseEvent>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    Helper helper;
    QTimer timer;
signals:

public slots:
    void animate();
    void paintEvent(QPaintEvent *event) override;

    double Dr_calculate(float disTance,float lValue,float theta);


private:

    int elapsed;

protected:
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // GLWIDGET_H
