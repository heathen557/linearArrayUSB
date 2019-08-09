#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QStandardItem>
#include<QStandardItemModel>
#include<QCheckBox>


bool isWriteSuccess;   //配置集是否配置成功标识
bool isRecvFlag;
int saveFileIndex;
QString saveFilePath;
bool isSaveFlag;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isWriteSuccess = false;
    isRecvFlag = false;
    isLinkSuccess = false;

    initTreeWidget();
    initGUI();
    initThread();
    initConnect();

}


void MainWindow::initConnect()
{
    //主函数与USB接收线程的 信号与槽的连接
    connect(this,SIGNAL(openLinkSignal(int,int)),recvUsbMsg_obj, SLOT(run(int,int)));
    connect(this,SIGNAL(closeLinkSignal()),recvUsbMsg_obj,SLOT(closeUSB()));
    connect(this,SIGNAL(read_usb_signal()),recvUsbMsg_obj,SLOT(read_usb()));
    connect(recvUsbMsg_obj,SIGNAL(linkInfoSignal(int)),this,SLOT(linkInfoSlot(int)));
    connect(this,SIGNAL(readSysSignal(int,bool)),recvUsbMsg_obj,SLOT(readSysSlot(int,bool)));
    connect(this,SIGNAL(writeSysSignal(int,QString,bool)),recvUsbMsg_obj,SLOT(writeSysSlot(int,QString,bool)));
    connect(this,SIGNAL(readDevSignal(int,int,bool)),recvUsbMsg_obj,SLOT(readDevSlot(int,int,bool)));
    connect(this,SIGNAL(writeDevSignal(int,int,QString,bool)),recvUsbMsg_obj,SLOT(writeDevSlot(int,int,QString,bool)));
    connect(this,SIGNAL(loadSettingSignal(QString,bool)),recvUsbMsg_obj,SLOT(loadSettingSlot(QString,bool)));
    connect(this,SIGNAL(saveSettingSignal(QString,int,bool)),recvUsbMsg_obj,SLOT(saveSettingSlot(QString,int,bool)));
    connect(recvUsbMsg_obj,SIGNAL(reReadSysSignal(QString)),this,SLOT(reReadSysSlot(QString)));
    connect(recvUsbMsg_obj,SIGNAL(reReadDevSignal(QString)),this,SLOT(reReadDevSlot(QString)));

    //主函数与数据处理线程的 信号与槽的连接
    connect(dealUsbMsg_obj,SIGNAL(statisticsValueSignal(float,float,float,float)),this,SLOT(statisticsValueSlot(float, float, float,float)));

    //文件保存相关的 信号与槽的连接
    connect(ui->action,SIGNAL(triggered()),this,SLOT(showSaveFileDialog())); //文件保存 窗口打开
    connect(&fileSaveDia,SIGNAL(isSaveFlagSignal(bool,QString,int)),this,SLOT(isSaveFlagSlot(bool,QString,int)));
    connect(dealUsbMsg_obj,SIGNAL(saveTXTSignal(QString)),savePCD_obj,SLOT(saveTXTSlot(QString)));

    //数据接收与数据处理线程 信号与槽的连接
    connect(recvUsbMsg_obj,SIGNAL(recvMsgSignal(QByteArray)),dealUsbMsg_obj,SLOT(recvMsgSlot(QByteArray)));



}


//要开启的线程的初始化函数,并且开启线程
void MainWindow::initThread()
{
    //USB数据处理线程
     dealUsbMsg_obj = new DealUsb_msg();
     dealUsbThread = new QThread;
     dealUsbMsg_obj->moveToThread(dealUsbThread);
     dealUsbThread->start();

    //把读取USB信息放到线程当中，并开启线程
    recvUsbMsg_obj = new ReceUSB_Msg();
    recvUsbThread = new QThread;
    recvUsbMsg_obj->moveToThread(recvUsbThread);
    recvUsbThread->start();

    //开启保存pcd文件的线程
    savePCD_obj = new savePCDThread;
    saveThread = new QThread;
    savePCD_obj->moveToThread(saveThread);
    saveThread->start();
}

//界面的初始化函数
void MainWindow::initGUI()
{
//    ui->textEdit->setAlignment(Qt::AlignHCenter);
    ui->textEdit->setAlignment(Qt::AlignJustify);



    ui->tableWidget_2->setColumnWidth(0,93);
    ui->tableWidget_2->setColumnWidth(1,93);
    ui->tableWidget_2->setRowHeight(0,43);
    ui->tableWidget_2->setRowHeight(1,44);
    tofMinItem_value.setTextAlignment(Qt::AlignCenter);
    tofMaxItem_value.setTextAlignment(Qt::AlignCenter);
    peakMinItem_value.setTextAlignment(Qt::AlignCenter);
    peakMaxItem_value.setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_2->setItem(0,0,&tofMinItem_value);
    ui->tableWidget_2->setItem(0,1,&tofMaxItem_value);
    ui->tableWidget_2->setItem(1,0,&peakMinItem_value);
    ui->tableWidget_2->setItem(1,1,&peakMaxItem_value);


}

//设备寄存器读写的界面的初始化函数
void MainWindow::initTreeWidget()
{
    connect(&pushButton,SIGNAL(clicked()),this,SLOT(pushButton_slot()));
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setColumnWidth(0,100);
    ui->treeWidget->setColumnWidth(1,50);
    ui->treeWidget->setColumnWidth(2,50);
    ui->treeWidget->setHeaderLabel(tr("item tree"));    //设置标题
    QList<QTreeWidgetItem *> items;
    //创建两个节点
    QTreeWidgetItem *fItem1 = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("TDC Setting")));
    QTreeWidgetItem *fItem2 = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Integration Setting")));
    items.append(fItem1);
    items.append(fItem2);
    //添加顶层节点
    ui->treeWidget->insertTopLevelItems(0,items);
    //节点始终保持展开
//    ui->treeWidget->setItemsExpandable(false);
//    ui->treeWidget->expandAll();
    ui->treeWidget->expandItem(fItem1);
    // ----------
    //创建子节点
    QTreeWidgetItem *fItem1a = new QTreeWidgetItem(fItem1,QStringList(QString("f1a")));
    QTreeWidgetItem *fItem1b = new QTreeWidgetItem(fItem1,QStringList(QString("f1b")));

    QTreeWidgetItem *fItem2a = new QTreeWidgetItem(fItem2,QStringList(QString("f2a")));
    QTreeWidgetItem *fItem2b = new QTreeWidgetItem(fItem2,QStringList(QString("f2b")));

        //添加字节点
    fItem1->addChild(fItem1a);
    fItem1->addChild(fItem1b);

    fItem2->addChild(fItem2a);
    fItem2->addChild(fItem2b);

    QCheckBox *box = new QCheckBox();
    box->setText("sss");
    QLineEdit *edit = new QLineEdit();
    pushButton.setText(QStringLiteral("读取"));
    ui->treeWidget->setItemWidget(fItem1a,1,edit);
    ui->treeWidget->setItemWidget(fItem1a,2,&pushButton);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButton_slot()
{
    qDebug()<<"have come into the slot"<<endl;
}

//连接USB设备槽函数
void MainWindow::on_linkUSB_pushButton_clicked()
{
    if(ui->linkUSB_pushButton->text() == QStringLiteral("连接设备"))
    {
        int vId = ui->VID_lineEdit->text().toInt(NULL,16);
        int pId = ui->PID_lineEdit->text().toInt(NULL,16);
        emit openLinkSignal(vId,pId);

        if(isWriteSuccess)      //关闭连接以后，重新连接后需直接接收数据
        {
            isRecvFlag = true;
            emit read_usb_signal();
        }
    }else if(ui->linkUSB_pushButton->text() == QStringLiteral("关闭连接"))
    {
        isRecvFlag = false;
        emit closeLinkSignal();
        ui->linkUSB_pushButton->setText(QStringLiteral("连接设备"));
        isLinkSuccess = false;


        /***打印日志部分**/
    }
}

//系统寄存器读取槽函数
void MainWindow::on_readSys_pushButton_clicked()
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    int address = ui->sysAddress_lineEdit->text().toInt(NULL,16);

    if(isRecvFlag)
    {
        isRecvFlag = false;
        emit readSysSignal(address,true);
    }else
    {
        emit readSysSignal(address,false);
    }
}

//接收系统指令返回信息槽函数
void MainWindow::reReadSysSlot(QString str)
{
    int m = str.toInt();
    qDebug()<<" the data =  "<<m<<endl;
    ui->sysData_lineEdit->setText(QString::number(m,16));
}

//系统寄存器写入槽函数
void MainWindow::on_writeSys_pushButton_clicked()
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    int address = ui->sysAddress_lineEdit->text().toInt(NULL,16);
    QString data = ui->sysData_lineEdit->text();

    if(isRecvFlag)
    {
        isRecvFlag = false;
        emit writeSysSignal(address,data,true);
    }else
    {
        emit writeSysSignal(address,data,false);
    }
    ui->sysData_lineEdit->clear();
}

//加载配置集槽函数
void MainWindow::on_loadSetting_pushButton_clicked()
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }

    QString file_path;
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("请选择配置文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径

    QStringList mimeTypeFilters;
    mimeTypeFilters <<QStringLiteral("芯片配置文件(*.para)|*.para") ;
    fileDialog->setNameFilters(mimeTypeFilters);


    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }else
    {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////////////

    file_path = fileNames[0];
    qDebug()<<" file_path = "<<fileNames[0]<<endl;

    QString checkStr = file_path.right(4);
    if("para" != checkStr)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("请选择正确的配置文件！"));
        return ;
    }

    //如果接收线程正在运行，先关闭接收线程（while循环），否则线程接收不到信号
    //线程处理完数据以后，再次打开while循环，即另isRecvFlag = true;

//    emit loadSettingSignal(file_path);

    if(isRecvFlag)
    {
        isRecvFlag = false;
        emit loadSettingSignal(file_path,true);
    }else
    {
        emit loadSettingSignal(file_path,false);
    }


}

//保存配置集槽函数
void MainWindow::on_saveSetting_pushButton_clicked()
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }

    QString filePath;

    QFileDialog *fileDialog = new QFileDialog(this);//创建一个QFileDialog对象，构造函数中的参数可以有所添加。
    fileDialog->setWindowTitle(tr("Save As"));//设置文件保存对话框的标题
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);//设置文件对话框为保存模式
    fileDialog->setFileMode(QFileDialog::AnyFile);//设置文件对话框弹出的时候显示任何文件，不论是文件夹还是文件
    fileDialog->setViewMode(QFileDialog::Detail);//文件以详细的形式显示，显示文件名，大小，创建日期等信息；
    fileDialog->setGeometry(10,30,300,200);//设置文件对话框的显示位置
    fileDialog->setDirectory(".");//设置文件对话框打开时初始打开的位置
    QStringList mimeTypeFilters;
    mimeTypeFilters <<QStringLiteral("芯片配置文件(*.para)|*.para") ;
    fileDialog->setNameFilters(mimeTypeFilters);


    if(fileDialog->exec() == QDialog::Accepted)
    {
        filePath = fileDialog->selectedFiles()[0];//得到用户选择的文件名
        qDebug()<<" filePath = "<<filePath<<endl;
    }else
    {
        return ;
    }

//    int deviceId = ui->lineEdit_3->text().toInt(NULL,16);
    //0xD8
    int deviceId = 216;


    //如果接收线程正在运行，先关闭接收线程（while循环），否则线程接收不到信号
    //线程处理完数据以后，再次打开while循环，即另isRecvFlag = true;
    if(isRecvFlag)
    {
        isRecvFlag = false;
        emit saveSettingSignal(filePath, deviceId, true);
    }else
    {
        emit saveSettingSignal(filePath, deviceId, false);
    }
}

//播放的槽函数
void MainWindow::on_pushButton_5_clicked()
{

    ui->widget->timer.start(10);
}

//读取设备指令返回信息的槽函数
void MainWindow::reReadDevSlot(QString str)
{

}

//USB连接信号响应的槽函数
void MainWindow::linkInfoSlot(int flagNum)
{
    QString strLog;
    QString tempStr;
    QTime t1 = QTime::currentTime();
    QString tempstr_1;
    QString str;

    switch (flagNum) {
    case 0:
        tempStr = QStringLiteral("设备连接成功");
        tempStr.append("                           ");

        ui->linkUSB_pushButton->setText(QStringLiteral("关闭连接"));
        isLinkSuccess = true;
        break;
    case 1:
        tempStr = QStringLiteral("未找到设备！");
        tempStr.append("                   ");
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("未找到设备！"));
        break;
    case 2:
        isRecvFlag = false ;
        tempStr = QStringLiteral("未接收到数据，请检查设备！");
        tempStr.append("             ");
//        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("未接收到数据，请检查设备，"));
        break;
    case 3:
        tempStr = QStringLiteral("打开设备失败！");
        tempStr.append("                   ");
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("打开设备失败"));
        break;
    case 4:
        tempStr = QStringLiteral("读取系统寄存器成功！");
        tempStr.append("                   ");
        break;
    case 5:
        tempStr = QStringLiteral("读取系统寄存器失败！");
        tempStr.append("                   ");
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("读取系统寄存器失败,"));
        break;
    case 6:
        tempStr = QStringLiteral("读取设备寄存器成功！");
        tempStr.append("                   ");
        break;
    case 7:
        tempStr = QStringLiteral("读取设备寄存器失败！");
        tempStr.append("                   ");
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("读取设备寄存器失败."));
        break;
    case 8:
        tempStr = QStringLiteral("加载配置信息成功！");
        tempStr.append("                     ");
        isWriteSuccess = true;
        break;
    case 9:
        tempStr = QStringLiteral("加载配置信息失败！");
        tempStr.append("                   ");
        isWriteSuccess = false;
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("加载配置信息失败."));
        break;
    case 10:
        tempStr = QStringLiteral("保存配置信息成功！");
        tempStr.append("                   ");
        break;
    case 11:
        tempStr = QStringLiteral("保存配置信息失败！");
        tempStr.append("                   ");
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("保存配置信息失败."));
        break;
    case 12:
        tempStr = QStringLiteral("写入系统寄存器成功！");
        tempStr.append("                   ");
        break;
    case 13:
        tempStr = QStringLiteral("写入系统寄存器失败！");
        tempStr.append("                   ");
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("写入系统寄存器失败的"));
        break;
    case 14:
        tempStr = QStringLiteral("写入设备寄存器成功！");
        tempStr.append("                   ");
        break;
    case 15:
        tempStr = QStringLiteral("写入设备寄存器失败！");
        tempStr.append("                   ");
        break;

    default:
        break;
    }
    str = tempStr  +t1.toString("hh:mm:ss");
    ui->textEdit->append(str);
}


//打开文件保存窗口
void MainWindow::showSaveFileDialog()
{
    fileSaveDia.show();
}

//用户保存信息接收 槽函数,formatSelect属于预留，暂时不适用
void MainWindow::isSaveFlagSlot(bool saveFlag, QString filePath,int formatSelect)
{
    if(saveFlag)
        saveFileIndex = 1;

    saveFilePath = filePath;
    isSaveFlag = saveFlag;
    qDebug()<<"saveFlag ="<<saveFlag<<"  filePath="<<filePath<<endl;

}


void MainWindow::statisticsValueSlot(float tofMin,float tofMax,float peakMin,float peakMax)
{
    tofMinItem_value.setText(QString::number(tofMin));
    tofMaxItem_value.setText(QString::number(tofMax));
    peakMinItem_value.setText(QString::number(peakMin));
    peakMaxItem_value.setText(QString::number(peakMax));
}
