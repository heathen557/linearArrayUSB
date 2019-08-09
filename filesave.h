#ifndef FILESAVE_H
#define FILESAVE_H

#include <QDialog>
#include<QFileDialog>
#include<QDebug>
#include<QMessageBox>

namespace Ui {
class fileSave;
}

class fileSave : public QDialog
{
    Q_OBJECT

public:
    explicit fileSave(QWidget *parent = 0);
    ~fileSave();

    QString file_path;

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_SelFilePath_pushButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

signals:

    void isSaveFlagSignal(bool,QString,int);  //是否保存pcd文件,路径、格式

private:
    Ui::fileSave *ui;
};

#endif // FILESAVE_H
