#ifndef SHOWSETTINGDIA_H
#define SHOWSETTINGDIA_H

#include <QDialog>

namespace Ui {
class showSettingDia;
}

class showSettingDia : public QDialog
{
    Q_OBJECT

public:
    explicit showSettingDia(QWidget *parent = 0);
    ~showSettingDia();

private slots:
    void on_ok_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    Ui::showSettingDia *ui;

signals:
    void showSettingParaSignal(int,int);
};

#endif // SHOWSETTINGDIA_H
