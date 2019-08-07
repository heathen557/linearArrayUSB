#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include<helper.h>
#include <QPainter>
#include <QTimer>

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


private:

    int elapsed;

};

#endif // GLWIDGET_H
