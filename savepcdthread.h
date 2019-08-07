#ifndef SAVEPCDTHREAD_H
#define SAVEPCDTHREAD_H

#include <QObject>

class savePCDThread : public QObject
{
    Q_OBJECT
public:
    explicit savePCDThread(QObject *parent = 0);

signals:

public slots:


    void writeTXT(QString text,int index);

    void saveTXTSlot(QString msgStr);
};

#endif // SAVEPCDTHREAD_H
