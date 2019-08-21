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

int showFrameNum;   //同时显示多少帧数据
int showTOFmax;     //设置显示的最大范围（m）

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isWriteSuccess = false;
    isRecvFlag = false;
    isLinkSuccess = false;

    showFrameNum = 1;
    showTOFmax = 10;


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
    connect(recvUsbMsg_obj,SIGNAL(reReadDevSignal(int,QString)),this,SLOT(reReadDevSlot(int,QString)));

    //主函数与数据处理线程的 信号与槽的连接
    connect(dealUsbMsg_obj,SIGNAL(statisticsValueSignal(float,float,float,float)),this,SLOT(statisticsValueSlot(float, float, float,float)));

    //文件保存相关的 信号与槽的连接
    connect(ui->action,SIGNAL(triggered()),this,SLOT(showSaveFileDialog())); //文件保存 窗口打开
    connect(&fileSaveDia,SIGNAL(isSaveFlagSignal(bool,QString,int)),this,SLOT(isSaveFlagSlot(bool,QString,int)));
    connect(dealUsbMsg_obj,SIGNAL(saveTXTSignal(QString)),savePCD_obj,SLOT(saveTXTSlot(QString)));

    //数据接收与数据处理线程 信号与槽的连接
    connect(recvUsbMsg_obj,SIGNAL(recvMsgSignal(QByteArray)),dealUsbMsg_obj,SLOT(recvMsgSlot(QByteArray)));

    //显示相关
    connect(&oneSecondTimer,SIGNAL(timeout()),this,SLOT(oneSecondTimer_slot()));  //1sec 刷新显示
    connect(ui->action_2,SIGNAL(triggered()),this,SLOT(showShowSettingDialog()));
    connect(&showSettingDia_,SIGNAL(showSettingParaSignal(int,int)),this,SLOT(showSettingParaSlot(int,int)));


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
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(4);
    ui->treeWidget->setColumnWidth(0,170);
    ui->treeWidget->setColumnWidth(1,35);
    ui->treeWidget->setColumnWidth(2,35);
    ui->treeWidget->setColumnWidth(3,35);

    QStringList strList;
    strList.append(QStringLiteral("寄存器名称"));
    strList.append(QStringLiteral("数据"));
    strList.append(QStringLiteral("读取"));
    strList.append(QStringLiteral("写入"));

    ui->treeWidget->setHeaderLabels(strList);

    QList<QTreeWidgetItem *> items;
    //创建两个节点
    QTreeWidgetItem *TDC = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("TDC")));
    QTreeWidgetItem *Integration = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Integration")));
    QTreeWidgetItem *MA = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("MA")));
    QTreeWidgetItem *Digital = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Digital")));
    QTreeWidgetItem *Analog = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Analog")));
    QTreeWidgetItem *Pixel = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Pixel")));
    QTreeWidgetItem *Top = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Top")));
    QTreeWidgetItem *Delayline = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Delayline")));
    QTreeWidgetItem *MISC = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("MISC")));
    QTreeWidgetItem *Others = new QTreeWidgetItem(ui->treeWidget,QStringList(QString("Others")));

    items.append(TDC);
    items.append(Integration);
    items.append(MA);
    items.append(Digital);
    items.append(Analog);
    items.append(Pixel);
    items.append(Top);
    items.append(Delayline);
    items.append(MISC);
    //添加顶层节点
    ui->treeWidget->insertTopLevelItems(0,items);

    int i = 0;


    //创建TDC子节点
    QTreeWidgetItem *TDC_widgetItem[13];
    TDC_widgetItem[0] = new QTreeWidgetItem(TDC,QStringList(QString("sfw_rst(0)[0]")));
    TDC_widgetItem[1] = new QTreeWidgetItem(TDC,QStringList(QString("r_cnt_rst_dly1(1)[7:4]")));
    TDC_widgetItem[2] = new QTreeWidgetItem(TDC,QStringList(QString("r_syncnt_rst_width(1)[3:0]")));
    TDC_widgetItem[3] = new QTreeWidgetItem(TDC,QStringList(QString("r_cnt_hld_dly2(2)[7:0]")));
    TDC_widgetItem[4] = new QTreeWidgetItem(TDC,QStringList(QString("r_tdc_rdck_dly1(3)[7:4]")));
    TDC_widgetItem[5] = new QTreeWidgetItem(TDC,QStringList(QString("r_tdc_redn_dly(3)[3:0]")));
    TDC_widgetItem[6] = new QTreeWidgetItem(TDC,QStringList(QString("r_tdc_cnt_rst_dly2(4)[7:4]")));
    TDC_widgetItem[7] = new QTreeWidgetItem(TDC,QStringList(QString("r_tdc_rdck_cyc(4)[3:0]")));
    TDC_widgetItem[8] = new QTreeWidgetItem(TDC,QStringList(QString("r_rising_latch(5)[7]")));
    TDC_widgetItem[9] = new QTreeWidgetItem(TDC,QStringList(QString("r_tdc_read_en_same(5)[6]")));
    TDC_widgetItem[10] = new QTreeWidgetItem(TDC,QStringList(QString("r_slower_clk(5)[5]")));
    TDC_widgetItem[11] = new QTreeWidgetItem(TDC,QStringList(QString("r_faster_clk(5)[4]")));
    TDC_widgetItem[12] = new QTreeWidgetItem(TDC,QStringList(QString("r_cnt_hld_dly1(5)[3:0]")));

    TDC_read_signalMapper = new QSignalMapper(this);
    TDC_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<13; i++)
    {
        TDC_lineEdit[i].setAlignment(Qt::AlignCenter);
        TDC_read_pushButton[i].setText(QStringLiteral("读取"));
        TDC_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(TDC_widgetItem[i],1,&TDC_lineEdit[i]);
        ui->treeWidget->setItemWidget(TDC_widgetItem[i],2,&TDC_read_pushButton[i]);
        ui->treeWidget->setItemWidget(TDC_widgetItem[i],3,&TDC_write_pushButton[i]);

        connect(&TDC_read_pushButton[i],SIGNAL(clicked()),TDC_read_signalMapper,SLOT(map()));
        TDC_read_signalMapper->setMapping(&TDC_read_pushButton[i],i);
        connect(&TDC_write_pushButton[i],SIGNAL(clicked()),TDC_write_signalMapper,SLOT(map()));
        TDC_write_signalMapper->setMapping(&TDC_write_pushButton[i],i);
    }
    connect(TDC_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(TDC_read_slot(int)));
    connect(TDC_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(TDC_write_slot(int)));


    //创建Integration子节点
    QTreeWidgetItem *Integration_widgetItem[4];
    Integration_widgetItem[0] = new QTreeWidgetItem(Integration,QStringList(QString("r_integ(6)[7:0]")));
    Integration_widgetItem[1] = new QTreeWidgetItem(Integration,QStringList(QString("r_hts(7)[5:4]")));
    Integration_widgetItem[2] = new QTreeWidgetItem(Integration,QStringList(QString("r_integ(7)[3:0]")));
    Integration_widgetItem[3] = new QTreeWidgetItem(Integration,QStringList(QString("r_hts(8)[7:0]")));
    Integration_read_signalMapper = new QSignalMapper(this);
    Integration_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<4; i++)
    {
        Integration_lineEdit[i].setAlignment(Qt::AlignCenter);
        Integration_read_pushButton[i].setText(QStringLiteral("读取"));
        Integration_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Integration_widgetItem[i],1,&Integration_lineEdit[i]);
        ui->treeWidget->setItemWidget(Integration_widgetItem[i],2,&Integration_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Integration_widgetItem[i],3,&Integration_write_pushButton[i]);

        connect(&Integration_read_pushButton[i],SIGNAL(clicked()),Integration_read_signalMapper,SLOT(map()));
        Integration_read_signalMapper->setMapping(&Integration_read_pushButton[i],i);
        connect(&Integration_write_pushButton[i],SIGNAL(clicked()),Integration_write_signalMapper,SLOT(map()));
        Integration_write_signalMapper->setMapping(&Integration_write_pushButton[i],i);
    }
    connect(Integration_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Integration_read_slot(int)));
    connect(Integration_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Integration_write_slot(int)));

    //创建MA 子节点
    QTreeWidgetItem *MA_widgetItem[16];
    MA_widgetItem[0] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w0(9)[3:0]")));
    MA_widgetItem[1] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w1(9)[7:4]")));
    MA_widgetItem[2] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w2(10)[3:0]")));
    MA_widgetItem[3] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w3(10)[7:4]")));
    MA_widgetItem[4] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w4(11)[3:0]")));
    MA_widgetItem[5] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w5(11)[7:4]")));
    MA_widgetItem[6] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w6(12)[3:0]")));
    MA_widgetItem[7] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w7(12)[7:4]")));
    MA_widgetItem[8] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w8(13)[3:0]")));
    MA_widgetItem[9] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w9(13)[7:4]")));
    MA_widgetItem[10] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wa(14)[3:0]")));
    MA_widgetItem[11] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wb(14)[7:4]")));
    MA_widgetItem[12] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wc(15)[3:0]")));
    MA_widgetItem[13] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wd(15)[7:4]")));
    MA_widgetItem[14] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_we(16)[3:0]")));
    MA_widgetItem[15] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wf(16)[7:4]")));
    MA_read_signalMapper = new QSignalMapper(this);
    MA_write_signalMapper = new QSignalMapper(this);

    for(i=0; i<16; i++)
    {
        MA_lineEdit[i].setAlignment(Qt::AlignCenter);
        MA_read_pushButton[i].setText(QStringLiteral("读取"));
        MA_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(MA_widgetItem[i],1,&MA_lineEdit[i]);
        ui->treeWidget->setItemWidget(MA_widgetItem[i],2,&MA_read_pushButton[i]);
        ui->treeWidget->setItemWidget(MA_widgetItem[i],3,&MA_write_pushButton[i]);

        connect(&MA_read_pushButton[i],SIGNAL(clicked()),MA_read_signalMapper,SLOT(map()));
        MA_read_signalMapper->setMapping(&MA_read_pushButton[i],i);
        connect(&MA_write_pushButton[i],SIGNAL(clicked()),MA_write_signalMapper,SLOT(map()));
        MA_write_signalMapper->setMapping(&MA_write_pushButton[i],i);
    }
    connect(MA_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(MA_read_slot(int)));
    connect(MA_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(MA_write_slot(int)));


    //创建Digital子节点
    QTreeWidgetItem *Digital_widgetItem[10];
    Digital_widgetItem[0] = new QTreeWidgetItem(Digital,QStringList(QString("r_spi_en(17)[7]")));
    Digital_widgetItem[1] = new QTreeWidgetItem(Digital,QStringList(QString("r_dvp_clk_sel(17)[6:4]")));
    Digital_widgetItem[2] = new QTreeWidgetItem(Digital,QStringList(QString("r_clk_divider(17)[3:0]")));
    Digital_widgetItem[3] = new QTreeWidgetItem(Digital,QStringList(QString("r_sramout_clksel(18)[7]")));
    Digital_widgetItem[4] = new QTreeWidgetItem(Digital,QStringList(QString("r_raw_out_mode(18)[6]")));
    Digital_widgetItem[5] = new QTreeWidgetItem(Digital,QStringList(QString("r_dvp_sram_output_mode(18)[5]")));
    Digital_widgetItem[6] = new QTreeWidgetItem(Digital,QStringList(QString("r_sram_output_cycles(18)[4:0]")));
    Digital_widgetItem[7] = new QTreeWidgetItem(Digital,QStringList(QString("r_row_start(19)[4:0]")));
    Digital_widgetItem[8] = new QTreeWidgetItem(Digital,QStringList(QString("r_high_bits(19)[7]")));
    Digital_widgetItem[9] = new QTreeWidgetItem(Digital,QStringList(QString("r_row_end(20)[5:0]")));
    Digital_read_signalMapper = new QSignalMapper(this);
    Digital_write_signalMapper= new QSignalMapper(this);
    for(i=0 ;i<10; i++)
    {
        Digital_lineEdit[i].setAlignment(Qt::AlignCenter);
        Digital_read_pushButton[i].setText(QStringLiteral("读取"));
        Digital_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Digital_widgetItem[i],1,&Digital_lineEdit[i]);
        ui->treeWidget->setItemWidget(Digital_widgetItem[i],2,&Digital_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Digital_widgetItem[i],3,&Digital_write_pushButton[i]);
        connect(&Digital_read_pushButton[i],SIGNAL(clicked()),Digital_read_signalMapper,SLOT(map()));
        Digital_read_signalMapper->setMapping(&Digital_read_pushButton[i],i);
        connect(&Digital_write_pushButton[i],SIGNAL(clicked()),Digital_write_signalMapper,SLOT(map()));
        Digital_write_signalMapper->setMapping(&Digital_write_pushButton[i],i);
    }
    connect(Digital_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Digital_read_slot(int)));
    connect(Digital_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Digital_write_slot(int)));



    //创建Analog子节点
    QTreeWidgetItem *Analog_widgetItem[26];
    Analog_widgetItem[0] = new QTreeWidgetItem(Analog,QStringList(QString("dl_sel_dly(21)[2:0]")));
    Analog_widgetItem[1] = new QTreeWidgetItem(Analog,QStringList(QString("dl_sel_long(21)[3]")));
    Analog_widgetItem[2] = new QTreeWidgetItem(Analog,QStringList(QString("dl_en(21)[4]")));
    Analog_widgetItem[3] = new QTreeWidgetItem(Analog,QStringList(QString("tdc_syncnt_en_global(21)[5]")));
    Analog_widgetItem[4] = new QTreeWidgetItem(Analog,QStringList(QString("tdc_ckdrv_en(21)[6]")));
    Analog_widgetItem[5] = new QTreeWidgetItem(Analog,QStringList(QString("sel_cnt_mode(21)[7]")));
    Analog_widgetItem[6] = new QTreeWidgetItem(Analog,QStringList(QString("pll_coarse_cnt_cksel(22)[4:0]")));
    Analog_widgetItem[7] = new QTreeWidgetItem(Analog,QStringList(QString("pll_lpf_rc(22)[7:6]")));
    Analog_widgetItem[8] = new QTreeWidgetItem(Analog,QStringList(QString("pll_div_ctrl(23)[6:0]")));
    Analog_widgetItem[9] = new QTreeWidgetItem(Analog,QStringList(QString("enb_pclk(24)[7]")));
    Analog_widgetItem[10] = new QTreeWidgetItem(Analog,QStringList(QString("r_tdc_start_re(24)[6]")));
    Analog_widgetItem[11] = new QTreeWidgetItem(Analog,QStringList(QString("mclk_div_rst(24)[5]")));
    Analog_widgetItem[12] = new QTreeWidgetItem(Analog,QStringList(QString("mclk_div_ctrl(24)[4:0]")));
    Analog_widgetItem[13] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[0](25)[0]")));
    Analog_widgetItem[14] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[3:1](25)[3:1]")));
    Analog_widgetItem[15] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[4](25)[4]")));
    Analog_widgetItem[16] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[7:5](25)[7:5]")));
    Analog_widgetItem[17] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[8](26)[0]")));
    Analog_widgetItem[18] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[12:9](26)[4:1]")));
    Analog_widgetItem[19] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[13](26)[5]")));
    Analog_widgetItem[20] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[15:14](26)[7:6]")));
    Analog_widgetItem[21] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[17:16](27)[1:0]")));
    Analog_widgetItem[22] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[19:18](27)[3:2]")));
    Analog_widgetItem[23] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[20](27)[4]")));
    Analog_widgetItem[24] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[22:21](27)[6:5]")));
    Analog_widgetItem[25] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out[23](27)[7]")));

    Analog_read_signalMapper = new QSignalMapper(this);
    Analog_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<26; i++)
    {
        Analog_lineEdit[i].setAlignment(Qt::AlignCenter);
        Analog_read_pushButton[i].setText(QStringLiteral("读取"));
        Analog_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Analog_widgetItem[i],1,&Analog_lineEdit[i]);
        ui->treeWidget->setItemWidget(Analog_widgetItem[i],2,&Analog_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Analog_widgetItem[i],3,&Analog_write_pushButton[i]);
        connect(&Analog_read_pushButton[i],SIGNAL(clicked()),Analog_read_signalMapper,SLOT(map()));
        Analog_read_signalMapper->setMapping(&Analog_read_pushButton[i],i);
        connect(&Analog_write_pushButton[i],SIGNAL(clicked()),Analog_write_signalMapper,SLOT(map()));
        Analog_write_signalMapper->setMapping(&Analog_write_pushButton[i],i);
    }
    connect(Analog_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Analog_read_slot(int)));
    connect(Analog_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Analog_write_slot(int)));



    //创建Pixel Setting 子节点
    QTreeWidgetItem *Pixel_widgetItem[17];
    Pixel_widgetItem[0] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_qch_bias_ctrl(28)[7:4]")));
    Pixel_widgetItem[1] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_cd_bias_ctrl(28)[3:0]")));
    Pixel_widgetItem[2] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_cntr_enb(29)[4]")));
    Pixel_widgetItem[3] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_bypass(29)[3]")));
    Pixel_widgetItem[4] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_th1(29)[2]")));
    Pixel_widgetItem[5] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_th0(29)[1]")));
    Pixel_widgetItem[6] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_mode(29)[0]")));
    Pixel_widgetItem[7] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_col_sel_2(30)[7:4]")));
    Pixel_widgetItem[8] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_col_sel_1(30)[3:0]")));
    Pixel_widgetItem[9] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[4:0](31)[4:0]")));
    Pixel_widgetItem[10] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[7:5](31)[7:5]")));
    Pixel_widgetItem[11] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[8](32)[0]")));
    Pixel_widgetItem[12] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[9](32)[1]")));
    Pixel_widgetItem[13] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[10](32)[2]")));
    Pixel_widgetItem[14] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[11](32)[3]")));
    Pixel_widgetItem[15] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[14:12](32)[6:4]")));
    Pixel_widgetItem[16] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out[15](32)[7]")));
    Pixel_read_signalMapper = new QSignalMapper(this);
    Pixel_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<17; i++)
    {
        Pixel_lineEdit[i].setAlignment(Qt::AlignCenter);
        Pixel_read_pushButton[i].setText(QStringLiteral("读取"));
        Pixel_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Pixel_widgetItem[i],1,&Pixel_lineEdit[i]);
        ui->treeWidget->setItemWidget(Pixel_widgetItem[i],2,&Pixel_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Pixel_widgetItem[i],3,&Pixel_write_pushButton[i]);
        connect(&Pixel_read_pushButton[i],SIGNAL(clicked()),Pixel_read_signalMapper,SLOT(map()));
        Pixel_read_signalMapper->setMapping(&Pixel_read_pushButton[i],i);
        connect(&Pixel_write_pushButton[i],SIGNAL(clicked()),Pixel_write_signalMapper,SLOT(map()));
        Pixel_write_signalMapper->setMapping(&Pixel_write_pushButton[i],i);
    }
    connect(Pixel_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Pixel_read_slot(int)));
    connect(Pixel_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Pixel_write_slot(int)));


    //创建Top Setting 子节点
    QTreeWidgetItem *Top_widgetItem[13];
    Top_widgetItem[0] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[3:0](33)[3:0]")));
    Top_widgetItem[1] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[4](33)[4]")));
    Top_widgetItem[2] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[7:5](33)[7:5]")));
    Top_widgetItem[3] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[8](34)[0]")));
    Top_widgetItem[4] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[12:9](34)[4:1]")));
    Top_widgetItem[5] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[15:13](34)[7:5]")));
    Top_widgetItem[6] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[18:16](35)[2:0]")));
    Top_widgetItem[7] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[23:19](35)[7:3]")));
    Top_widgetItem[8] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[24](36)[0]")));
    Top_widgetItem[9] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[26:25](36)[2:1]")));
    Top_widgetItem[10] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[28:27](36)[4:3]")));
    Top_widgetItem[11] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[30:29](36)[6:5]")));
    Top_widgetItem[12] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out[31](36)[7]")));

    Top_read_signalMapper = new QSignalMapper(this);
    Top_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<13; i++)
    {
        Top_lineEdit[i].setAlignment(Qt::AlignCenter);
        Top_read_pushButton[i].setText(QStringLiteral("读取"));
        Top_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Top_widgetItem[i],1,&Top_lineEdit[i]);
        ui->treeWidget->setItemWidget(Top_widgetItem[i],2,&Top_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Top_widgetItem[i],3,&Top_write_pushButton[i]);
        connect(&Top_read_pushButton[i],SIGNAL(clicked()),Top_read_signalMapper,SLOT(map()));
        Top_read_signalMapper->setMapping(&Top_read_pushButton[i],i);
        connect(&Top_write_pushButton[i],SIGNAL(clicked()),Top_write_signalMapper,SLOT(map()));
        Top_write_signalMapper->setMapping(&Top_write_pushButton[i],i);
    }
    connect(Top_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Top_read_slot(int)));
    connect(Top_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Top_write_slot(int)));


    //创建Delayline Setting 子节点
    QTreeWidgetItem *Delayline_widgetItem[7];
    Delayline_widgetItem[0] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_in_en(37)[7:3]")));
    Delayline_widgetItem[1] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_width(37)[2:0]")));
    Delayline_widgetItem[2] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_in_0(38)[7:0]")));
    Delayline_widgetItem[3] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_in_1(39)[7:0]")));
    Delayline_widgetItem[4] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_in_2(40)[7:0]")));
    Delayline_widgetItem[5] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_in_3(41)[7:0]")));
    Delayline_widgetItem[6] = new QTreeWidgetItem(Delayline,QStringList(QString("r_test_dl_in_4(42)[7:0]")));
    Delayline_read_signalMapper = new QSignalMapper(this);
    Delayline_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<7; i++)
    {
        Delayline_lineEdit[i].setAlignment(Qt::AlignCenter);
        Delayline_read_pushButton[i].setText(QStringLiteral("读取"));
        Delayline_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Delayline_widgetItem[i],1,&Delayline_lineEdit[i]);
        ui->treeWidget->setItemWidget(Delayline_widgetItem[i],2,&Delayline_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Delayline_widgetItem[i],3,&Delayline_write_pushButton[i]);
        connect(&Delayline_read_pushButton[i],SIGNAL(clicked()),Delayline_read_signalMapper,SLOT(map()));
        Delayline_read_signalMapper->setMapping(&Delayline_read_pushButton[i],i);
        connect(&Delayline_write_pushButton[i],SIGNAL(clicked()),Delayline_write_signalMapper,SLOT(map()));
        Delayline_write_signalMapper->setMapping(&Delayline_write_pushButton[i],i);
    }
    connect(Delayline_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Delayline_read_slot(int)));
    connect(Delayline_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Delayline_write_slot(int)));


    //创建MISC Setting 子节点
    QTreeWidgetItem *MISC_widgetItem[8];
    MISC_widgetItem[0] = new QTreeWidgetItem(MISC,QStringList(QString("r_tdc_start_rise_state(43)[7]")));
    MISC_widgetItem[1] = new QTreeWidgetItem(MISC,QStringList(QString("r_tdc_start_rise(43)[6:4]")));
    MISC_widgetItem[2] = new QTreeWidgetItem(MISC,QStringList(QString("r_tdc_start_fall_state(43)[3]")));
    MISC_widgetItem[3] = new QTreeWidgetItem(MISC,QStringList(QString("r_tdc_start_fall(43)[2:0]")));
    MISC_widgetItem[4] = new QTreeWidgetItem(MISC,QStringList(QString("r_pds(44)[7:0]")));
    MISC_widgetItem[5] = new QTreeWidgetItem(MISC,QStringList(QString("r_pds(45)[7:0]")));
    MISC_widgetItem[6] = new QTreeWidgetItem(MISC,QStringList(QString("r_pds(46)[7:0]")));
    MISC_widgetItem[7] = new QTreeWidgetItem(MISC,QStringList(QString("r_pds(47)[7:0]")));
    MISC_read_signalMapper = new QSignalMapper(this);
    MISC_write_signalMapper = new QSignalMapper(this);

    for(i=0; i<8; i++)
    {
        MISC_lineEdit[i].setAlignment(Qt::AlignCenter);
        MISC_read_pushButton[i].setText(QStringLiteral("读取"));
        MISC_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(MISC_widgetItem[i],1,&MISC_lineEdit[i]);
        ui->treeWidget->setItemWidget(MISC_widgetItem[i],2,&MISC_read_pushButton[i]);
        ui->treeWidget->setItemWidget(MISC_widgetItem[i],3,&MISC_write_pushButton[i]);
        connect(&MISC_read_pushButton[i],SIGNAL(clicked()),MISC_read_signalMapper,SLOT(map()));
        MISC_read_signalMapper->setMapping(&MISC_read_pushButton[i],i);
        connect(&MISC_write_pushButton[i],SIGNAL(clicked()),MISC_write_signalMapper,SLOT(map()));
        MISC_write_signalMapper->setMapping(&MISC_write_pushButton[i],i);
    }
    connect(MISC_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(MISC_read_slot(int)));
    connect(MISC_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(MISC_write_slot(int)));


    //创建Others Setting 子节点
    QTreeWidgetItem *Others_widgetItem[7];
    Others_widgetItem[0] = new QTreeWidgetItem(Others,QStringList(QString("pll_pwdn(48)[7]")));
    Others_widgetItem[1] = new QTreeWidgetItem(Others,QStringList(QString("pll_test_en(48)[6]")));
    Others_widgetItem[2] = new QTreeWidgetItem(Others,QStringList(QString("pll_div_rst(48)[5]")));
    Others_widgetItem[3] = new QTreeWidgetItem(Others,QStringList(QString("r_reduce_cnt(48)[4]")));
    Others_widgetItem[4] = new QTreeWidgetItem(Others,QStringList(QString("Noise_reg(48)[3:0]")));
    Others_widgetItem[5] = new QTreeWidgetItem(Others,QStringList(QString("rhigh_hts(49)[7:0]")));

    Others_read_signalMapper = new QSignalMapper(this);
    Others_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<6; i++)
    {
        Others_lineEdit[i].setAlignment(Qt::AlignCenter);
        Others_read_pushButton[i].setText(QStringLiteral("读取"));
        Others_write_pushButton[i].setText(QStringLiteral("写入"));
        ui->treeWidget->setItemWidget(Others_widgetItem[i],1,&Others_lineEdit[i]);
        ui->treeWidget->setItemWidget(Others_widgetItem[i],2,&Others_read_pushButton[i]);
        ui->treeWidget->setItemWidget(Others_widgetItem[i],3,&Others_write_pushButton[i]);
        connect(&Others_read_pushButton[i],SIGNAL(clicked()),Others_read_signalMapper,SLOT(map()));
        Others_read_signalMapper->setMapping(&Others_read_pushButton[i],i);
        connect(&Others_write_pushButton[i],SIGNAL(clicked()),Others_write_signalMapper,SLOT(map()));
        Others_write_signalMapper->setMapping(&Others_write_pushButton[i],i);
    }
    connect(Others_read_signalMapper,SIGNAL(mapped(int)),this,SLOT(Others_read_slot(int)));
    connect(Others_write_signalMapper,SIGNAL(mapped(int)),this,SLOT(Others_write_slot(int)));


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
    if(ui->pushButton_5->text() == QStringLiteral("播放"))
    {
        ui->widget->timer.start(10);
        ui->pushButton_5->setText(QStringLiteral("暂停"));
        oneSecondTimer.start(1000);

    }else if(ui->pushButton_5->text() == QStringLiteral("暂停"))
    {
        ui->widget->timer.stop();
        ui->pushButton_5->setText(QStringLiteral("播放"));
        oneSecondTimer.stop();
    }
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

//接收线程发送来的统计信息的槽函数
void MainWindow::statisticsValueSlot(float tofMin,float tofMax,float peakMin,float peakMax)
{
    tofMin_ = tofMin;
    tofMax_ = tofMax;
    peakMin_ = peakMin;
    peakMax_ = peakMax;

}

//显示统计信息槽函数
void MainWindow::oneSecondTimer_slot()
{
    tofMinItem_value.setText(QString::number(tofMin_));
    tofMaxItem_value.setText(QString::number(tofMax_));
    peakMinItem_value.setText(QString::number(peakMin_));
    peakMaxItem_value.setText(QString::number(peakMax_));
}

//显示窗口弹出槽函数
void MainWindow::showShowSettingDialog()
{
    showSettingDia_.show();
}

//接收显示设置参数的槽函数
void MainWindow::showSettingParaSlot(int FrameNum,int TOFmax)
{
    showFrameNum = FrameNum;
    showTOFmax = TOFmax;

    qDebug()<<"showFrameNum = "<<showFrameNum<<"  showTOFmax ="<<showTOFmax<<endl;
}


/**************************单个寄存器配置相关的槽函数***************************************************/
//读取TDC 槽函数   TDC_number 在节点中的序号
void MainWindow::TDC_read_slot(int TDC_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    int hardWareAddress = 216;
    qDebug()<<"read TDC_number = "<<TDC_number<<endl;
    switch (TDC_number) {
    case 0:
        emit readDevSignal(hardWareAddress,0,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,1,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,1,false);
    case 3:
        emit readDevSignal(hardWareAddress,2,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,3,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,3,false);
    case 6:
        emit readDevSignal(hardWareAddress,4,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,4,false);
        break;
    case 8:
        emit readDevSignal(hardWareAddress,5,false);
    case 9:
        emit readDevSignal(hardWareAddress,5,false);
        break;
    case 10:
        emit readDevSignal(hardWareAddress,5,false);
        break;
    case 11:
        emit readDevSignal(hardWareAddress,5,false);
        break;
    case 12:
        emit readDevSignal(hardWareAddress,5,false);
        break;
    default:
        break;
    }

}
//写入TDC 槽函数
//对于写入寄存器 address resisterAddress 都是十进制数   data是十六进制数
void MainWindow::TDC_write_slot(int TDC_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }

    qDebug()<<"write TDC_number = "<<TDC_number<<endl;
    int hardWareAddress = 216;
    switch (TDC_number) {
    case 0:{
        int data = TDC_lineEdit[0].text().toInt(NULL,16);   //此命令只指定最低位，故不需要做位操作
        emit writeDevSignal(hardWareAddress, 0 , QString::number(data,16), false);
        TDC_lineEdit[0].setText("");
        break;
    }case 1:{
        int data = (TDC_lineEdit[1].text().toInt(NULL,16)<<4)+ TDC_lineEdit[2].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 1 , QString::number(data,16), false);
        TDC_lineEdit[1].setText("");
        TDC_lineEdit[2].setText("");
        break;
    }case 2:{
        int data = (TDC_lineEdit[1].text().toInt(NULL,16)<<4)+ TDC_lineEdit[2].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 1 , QString::number(data,16), false);
        TDC_lineEdit[1].setText("");
        TDC_lineEdit[2].setText("");
        break;
    }case 3:{
        int data = TDC_lineEdit[3].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 2 , QString::number(data,16), false);
        TDC_lineEdit[3].setText("");
        break;
    }case 4:{
        int data = (TDC_lineEdit[4].text().toInt(NULL,16)<<4)+ TDC_lineEdit[5].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 3 , QString::number(data,16), false);
        TDC_lineEdit[4].setText("");
        TDC_lineEdit[5].setText("");
        break;
    }case 5:{
        int data = (TDC_lineEdit[4].text().toInt(NULL,16)<<4)+ TDC_lineEdit[5].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 3 , QString::number(data,16), false);
        TDC_lineEdit[4].setText("");
        TDC_lineEdit[5].setText("");
    }case 6:{
        int data = (TDC_lineEdit[6].text().toInt(NULL,16)<<4)+ TDC_lineEdit[7].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 4 , QString::number(data,16), false);
        TDC_lineEdit[6].setText("");
        TDC_lineEdit[7].setText("");
        break;
    }case 7:{
        int data = (TDC_lineEdit[6].text().toInt(NULL,16)<<4)+ TDC_lineEdit[7].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 4 , QString::number(data,16), false);
        TDC_lineEdit[6].setText("");
        TDC_lineEdit[7].setText("");
        break;
    }case 8:{
        int data = (TDC_lineEdit[8].text().toInt(NULL,16)<<7)+(TDC_lineEdit[9].text().toInt(NULL,16)<<6)+
                (TDC_lineEdit[10].text().toInt(NULL,16)<<5) + (TDC_lineEdit[11].text().toInt(NULL,16)<<4)+
                TDC_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 5 , QString::number(data,16), false);
        TDC_lineEdit[8].setText("");
        TDC_lineEdit[9].setText("");
        TDC_lineEdit[10].setText("");
        TDC_lineEdit[11].setText("");
        TDC_lineEdit[12].setText("");
        break;
    }case 9:{
        int data = (TDC_lineEdit[8].text().toInt(NULL,16)<<7)+(TDC_lineEdit[9].text().toInt(NULL,16)<<6)+
                (TDC_lineEdit[10].text().toInt(NULL,16)<<5) + (TDC_lineEdit[11].text().toInt(NULL,16)<<4)+
                TDC_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 5 , QString::number(data,16), false);
        TDC_lineEdit[8].setText("");
        TDC_lineEdit[9].setText("");
        TDC_lineEdit[10].setText("");
        TDC_lineEdit[11].setText("");
        TDC_lineEdit[12].setText("");
        break;
    }case 10:{
        int data = (TDC_lineEdit[8].text().toInt(NULL,16)<<7)+(TDC_lineEdit[9].text().toInt(NULL,16)<<6)+
                (TDC_lineEdit[10].text().toInt(NULL,16)<<5) + (TDC_lineEdit[11].text().toInt(NULL,16)<<4)+
                TDC_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 5 , QString::number(data,16), false);
        TDC_lineEdit[8].setText("");
        TDC_lineEdit[9].setText("");
        TDC_lineEdit[10].setText("");
        TDC_lineEdit[11].setText("");
        TDC_lineEdit[12].setText("");
        break;
    }case 11:{
        int data = (TDC_lineEdit[8].text().toInt(NULL,16)<<7)+(TDC_lineEdit[9].text().toInt(NULL,16)<<6)+
                (TDC_lineEdit[10].text().toInt(NULL,16)<<5) + (TDC_lineEdit[11].text().toInt(NULL,16)<<4)+
                TDC_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 5 , QString::number(data,16), false);
        TDC_lineEdit[8].setText("");
        TDC_lineEdit[9].setText("");
        TDC_lineEdit[10].setText("");
        TDC_lineEdit[11].setText("");
        TDC_lineEdit[12].setText("");
        break;
    }case 12:{
        int data = (TDC_lineEdit[8].text().toInt(NULL,16)<<7)+(TDC_lineEdit[9].text().toInt(NULL,16)<<6)+
                (TDC_lineEdit[10].text().toInt(NULL,16)<<5) + (TDC_lineEdit[11].text().toInt(NULL,16)<<4)+
                TDC_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 5 , QString::number(data,16), false);
        TDC_lineEdit[8].setText("");
        TDC_lineEdit[9].setText("");
        TDC_lineEdit[10].setText("");
        TDC_lineEdit[11].setText("");
        TDC_lineEdit[12].setText("");
        qDebug()<<"ten jinzhi="<<data<<"    16jinzhi="<<QString::number(data,16)<<endl;
        break;
    }

    default:
        break;
    }
}



//读取Integration 槽函数
void MainWindow::Integration_read_slot(int Integration_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }

    int hardWareAddress = 216;
    qDebug()<<"read Integration_number = "<<Integration_number<<endl;
    switch (Integration_number) {
    case 0:
        emit readDevSignal(hardWareAddress,6,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,7,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,7,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,8,false);
        break;
    default:
        break;
    }
}
//写入读取Integration 槽函数
void MainWindow::Integration_write_slot(int Integration_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    int hardWareAddress = 216;
     qDebug()<<"write Integration_number = "<<Integration_number<<endl;

     switch (Integration_number) {
     case 0:{
         int data  = Integration_lineEdit[0].text().toInt(NULL,16);
         emit writeDevSignal(hardWareAddress, 6 , QString::number(data,16), false);
         Integration_lineEdit[0].setText("");
         break;
     }
     case 1:{
         int data = (Integration_lineEdit[1].text().toInt(NULL,16)<<4) + Integration_lineEdit[2].text().toInt(NULL,16);
         emit writeDevSignal(hardWareAddress, 7 , QString::number(data,16), false);
         Integration_lineEdit[1].setText("");
         Integration_lineEdit[2].setText("");
         break;
     }case 2:{
         int data = (Integration_lineEdit[1].text().toInt(NULL,16)<<4) + Integration_lineEdit[2].text().toInt(NULL,16);
         emit writeDevSignal(hardWareAddress, 7 , QString::number(data,16), false);
         Integration_lineEdit[1].setText("");
         Integration_lineEdit[2].setText("");
         break;
     }case 3:{
         int data = Integration_lineEdit[3].text().toInt(NULL,16);
         emit writeDevSignal(hardWareAddress, 8 , QString::number(data,16), false);
         Integration_lineEdit[3].setText("");
         break;
     }

     default:
         break;
     }
}


//读取 MA 槽函数
void MainWindow::MA_read_slot(int MA_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read MA_number = "<<MA_number<<endl;

    int hardWareAddress = 216;

    switch (MA_number) {
    case 0:
        emit readDevSignal(hardWareAddress,9,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,9,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,10,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,10,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,11,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,11,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,12,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,12,false);
        break;
    case 8:
        emit readDevSignal(hardWareAddress,13,false);
        break;
    case 9:
        emit readDevSignal(hardWareAddress,13,false);
        break;
    case 10:
        emit readDevSignal(hardWareAddress,14,false);
        break;
    case 11:
        emit readDevSignal(hardWareAddress,14,false);
        break;
    case 12:
        emit readDevSignal(hardWareAddress,15,false);
        break;
    case 13:
        emit readDevSignal(hardWareAddress,15,false);
        break;
    case 14:
        emit readDevSignal(hardWareAddress,16,false);
        break;
    case 15:
        emit readDevSignal(hardWareAddress,16,false);
        break;
    default:
        break;
    }


}
//写入 MA 槽函数
void MainWindow::MA_write_slot(int MA_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    int hardWareAddress = 216;
    qDebug()<<"write MA_number = "<<MA_number<<endl;

    switch (MA_number) {
    case 0:{
        int data  = MA_lineEdit[0].text().toInt(NULL,16) + (MA_lineEdit[1].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 9 , QString::number(data,16), false);
        MA_lineEdit[0].setText("");
        MA_lineEdit[1].setText("");
        break;
    }
    case 1:{
        int data  = MA_lineEdit[0].text().toInt(NULL,16) + (MA_lineEdit[1].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 9 , QString::number(data,16), false);
        MA_lineEdit[0].setText("");
        MA_lineEdit[1].setText("");
        break;
    }case 2:{
        int data = MA_lineEdit[2].text().toInt(NULL,16) + (MA_lineEdit[3].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 10 , QString::number(data,16), false);
        MA_lineEdit[2].setText("");
        MA_lineEdit[3].setText("");
        break;
    }case 3:{
        int data = MA_lineEdit[2].text().toInt(NULL,16) + (MA_lineEdit[3].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 10 , QString::number(data,16), false);
        MA_lineEdit[2].setText("");
        MA_lineEdit[3].setText("");
        break;
    }case 4:{
        int data = MA_lineEdit[4].text().toInt(NULL,16) + (MA_lineEdit[5].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 11 , QString::number(data,16), false);
        MA_lineEdit[4].setText("");
        MA_lineEdit[5].setText("");
        break;
    }case 5:{
        int data = MA_lineEdit[4].text().toInt(NULL,16) + (MA_lineEdit[5].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 11 , QString::number(data,16), false);
        MA_lineEdit[4].setText("");
        MA_lineEdit[5].setText("");
        break;
    }case 6:{
        int data = MA_lineEdit[6].text().toInt(NULL,16) + (MA_lineEdit[7].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 12 , QString::number(data,16), false);
        MA_lineEdit[6].setText("");
        MA_lineEdit[7].setText("");
        break;
    }case 7:{
        int data = MA_lineEdit[6].text().toInt(NULL,16) + (MA_lineEdit[7].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 12 , QString::number(data,16), false);
        MA_lineEdit[6].setText("");
        MA_lineEdit[7].setText("");
        break;
    }case 8:{
        int data = MA_lineEdit[8].text().toInt(NULL,16) + (MA_lineEdit[9].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 13 , QString::number(data,16), false);
        MA_lineEdit[8].setText("");
        MA_lineEdit[9].setText("");
        break;
    }case 9:{
        int data = MA_lineEdit[8].text().toInt(NULL,16) + (MA_lineEdit[9].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 13 , QString::number(data,16), false);
        MA_lineEdit[8].setText("");
        MA_lineEdit[9].setText("");
        break;
    }case 10:{
        int data = MA_lineEdit[10].text().toInt(NULL,16) + (MA_lineEdit[11].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 14 , QString::number(data,16), false);
        MA_lineEdit[10].setText("");
        MA_lineEdit[11].setText("");
        break;
    }case 11:{
        int data = MA_lineEdit[10].text().toInt(NULL,16) + (MA_lineEdit[11].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 14 , QString::number(data,16), false);
        MA_lineEdit[10].setText("");
        MA_lineEdit[11].setText("");
        break;
    }case 12:{
        int data = MA_lineEdit[12].text().toInt(NULL,16) + (MA_lineEdit[13].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 15 , QString::number(data,16), false);
        MA_lineEdit[12].setText("");
        MA_lineEdit[13].setText("");
        break;
    }case 13:{
        int data = MA_lineEdit[12].text().toInt(NULL,16) + (MA_lineEdit[13].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 15 , QString::number(data,16), false);
        MA_lineEdit[12].setText("");
        MA_lineEdit[13].setText("");
        break;
    }case 14:{
        int data = MA_lineEdit[14].text().toInt(NULL,16) + (MA_lineEdit[15].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 16 , QString::number(data,16), false);
        MA_lineEdit[14].setText("");
        MA_lineEdit[15].setText("");
        break;
    }case 15:{
        int data = MA_lineEdit[14].text().toInt(NULL,16) + (MA_lineEdit[15].text().toInt(NULL,16)<<4) ;
        emit writeDevSignal(hardWareAddress, 16 , QString::number(data,16), false);
        MA_lineEdit[14].setText("");
        MA_lineEdit[15].setText("");
        break;
    }

    default:
        break;
    }
}


//读取Digital槽函数
void MainWindow::Digital_read_slot(int Digital_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read Digital_number = "<<Digital_number<<endl;

    int hardWareAddress = 216;

    switch (Digital_number) {
    case 0:
        emit readDevSignal(hardWareAddress,17,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,17,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,17,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,18,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,18,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,18,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,18,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,19,false);
        break;
    case 8:
        emit readDevSignal(hardWareAddress,19,false);
        break;
    case 9:
        emit readDevSignal(hardWareAddress,20,false);
        break;
    default:
        break;
    }


}
//写入Digital槽函数
void MainWindow::Digital_write_slot(int Digital_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write Digital_number = "<<Digital_number<<endl;
    int hardWareAddress = 216;
    switch (Digital_number) {
    case 0:{
        int data  = (Digital_lineEdit[0].text().toInt(NULL,16)<<7) + (Digital_lineEdit[1].text().toInt(NULL,16)<<4) +
                Digital_lineEdit[2].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 17 , QString::number(data,16), false);
        Digital_lineEdit[0].setText("");
        Digital_lineEdit[1].setText("");
        Digital_lineEdit[2].setText("");
        break;
    }case 1:{
        int data  = (Digital_lineEdit[0].text().toInt(NULL,16)<<7) + (Digital_lineEdit[1].text().toInt(NULL,16)<<4) +
                Digital_lineEdit[2].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 17 , QString::number(data,16), false);
        Digital_lineEdit[0].setText("");
        Digital_lineEdit[1].setText("");
        Digital_lineEdit[2].setText("");
        break;
    }case 2:{
        int data  = (Digital_lineEdit[0].text().toInt(NULL,16)<<7) + (Digital_lineEdit[1].text().toInt(NULL,16)<<4) +
                Digital_lineEdit[2].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 17 , QString::number(data,16), false);
        Digital_lineEdit[0].setText("");
        Digital_lineEdit[1].setText("");
        Digital_lineEdit[2].setText("");
        break;
    }case 3:{
        int data = (Digital_lineEdit[3].text().toInt(NULL,16)<<7) + (Digital_lineEdit[4].text().toInt(NULL,16)<<6) +
                (Digital_lineEdit[5].text().toInt(NULL,16)<<5) + Digital_lineEdit[6].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 18 , QString::number(data,16), false);
        Digital_lineEdit[3].setText("");
        Digital_lineEdit[4].setText("");
        Digital_lineEdit[5].setText("");
        Digital_lineEdit[6].setText("");
        break;
    }case 4:{
        int data = (Digital_lineEdit[3].text().toInt(NULL,16)<<7) + (Digital_lineEdit[4].text().toInt(NULL,16)<<6) +
                (Digital_lineEdit[5].text().toInt(NULL,16)<<5) + Digital_lineEdit[6].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 18 , QString::number(data,16), false);
        Digital_lineEdit[3].setText("");
        Digital_lineEdit[4].setText("");
        Digital_lineEdit[5].setText("");
        Digital_lineEdit[6].setText("");
        break;
    }case 5:{
        int data = (Digital_lineEdit[3].text().toInt(NULL,16)<<7) + (Digital_lineEdit[4].text().toInt(NULL,16)<<6) +
                (Digital_lineEdit[5].text().toInt(NULL,16)<<5) + Digital_lineEdit[6].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 18 , QString::number(data,16), false);
        Digital_lineEdit[3].setText("");
        Digital_lineEdit[4].setText("");
        Digital_lineEdit[5].setText("");
        Digital_lineEdit[6].setText("");
        break;
    }case 6:{
        int data = (Digital_lineEdit[3].text().toInt(NULL,16)<<7) + (Digital_lineEdit[4].text().toInt(NULL,16)<<6) +
                (Digital_lineEdit[5].text().toInt(NULL,16)<<5) + Digital_lineEdit[6].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 18 , QString::number(data,16), false);
        Digital_lineEdit[3].setText("");
        Digital_lineEdit[4].setText("");
        Digital_lineEdit[5].setText("");
        Digital_lineEdit[6].setText("");
        break;
    }case 7:{
        int data = Digital_lineEdit[7].text().toInt(NULL,16) + (Digital_lineEdit[8].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 19 , QString::number(data,16), false);
        Digital_lineEdit[7].setText("");
        Digital_lineEdit[8].setText("");
        break;
    }case 8:{
        int data = Digital_lineEdit[7].text().toInt(NULL,16) + (Digital_lineEdit[8].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 19 , QString::number(data,16), false);
        Digital_lineEdit[7].setText("");
        Digital_lineEdit[8].setText("");
        break;
    }case 9:{
        int data = Digital_lineEdit[9].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 20 , QString::number(data,16), false);
        Digital_lineEdit[9].setText("");
        break;
    }




    }

}



//读取Analog槽函数
void MainWindow::Analog_read_slot(int Analog_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read Analog_number = "<<Analog_number<<endl;
    int hardWareAddress = 216;

    switch (Analog_number) {
    case 0:
        emit readDevSignal(hardWareAddress,21,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,21,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,21,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,21,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,21,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,21,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,22,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,22,false);
        break;
    case 8:
        emit readDevSignal(hardWareAddress,23,false);
        break;
    case 9:
        emit readDevSignal(hardWareAddress,24,false);
        break;
    case 10:
        emit readDevSignal(hardWareAddress,24,false);
        break;
    case 11:
        emit readDevSignal(hardWareAddress,24,false);
        break;
    case 12:
        emit readDevSignal(hardWareAddress,24,false);
        break;
    case 13:
        emit readDevSignal(hardWareAddress,25,false);
        break;
    case 14:
        emit readDevSignal(hardWareAddress,25,false);
        break;
    case 15:
        emit readDevSignal(hardWareAddress,25,false);
        break;
    case 16:
        emit readDevSignal(hardWareAddress,25,false);
        break;
    case 17:
        emit readDevSignal(hardWareAddress,26,false);
        break;
    case 18:
        emit readDevSignal(hardWareAddress,26,false);
        break;
    case 19:
        emit readDevSignal(hardWareAddress,26,false);
        break;
    case 20:
        emit readDevSignal(hardWareAddress,26,false);
        break;
    case 21:
        emit readDevSignal(hardWareAddress,27,false);
        break;
    case 22:
        emit readDevSignal(hardWareAddress,27,false);
        break;
    case 23:
        emit readDevSignal(hardWareAddress,27,false);
        break;
    case 24:
        emit readDevSignal(hardWareAddress,27,false);
        break;
    case 25:
        emit readDevSignal(hardWareAddress,27,false);
        break;
    default:
        break;

    }

}
//写入Analog槽函数
void MainWindow::Analog_write_slot(int Analog_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write Analog_number = "<<Analog_number<<endl;
    int hardWareAddress = 216;
    switch (Analog_number) {
    case 0:{
        int data = Analog_lineEdit[0].text().toInt(NULL,16) + (Analog_lineEdit[1].text().toInt(NULL,16)<<3) +
                   (Analog_lineEdit[2].text().toInt(NULL,16)<<4) + (Analog_lineEdit[3].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[4].text().toInt(NULL,16)<<6) + (Analog_lineEdit[5].text().toInt(NULL,16)<<7);

        emit writeDevSignal(hardWareAddress, 21 , QString::number(data,16), false);
        Analog_lineEdit[0].setText("");
        Analog_lineEdit[1].setText("");
        Analog_lineEdit[2].setText("");
        Analog_lineEdit[3].setText("");
        Analog_lineEdit[4].setText("");
        Analog_lineEdit[5].setText("");
        break;
    }case 1:{
        int data = Analog_lineEdit[0].text().toInt(NULL,16) + (Analog_lineEdit[1].text().toInt(NULL,16)<<3) +
                   (Analog_lineEdit[2].text().toInt(NULL,16)<<4) + (Analog_lineEdit[3].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[4].text().toInt(NULL,16)<<6) + (Analog_lineEdit[5].text().toInt(NULL,16)<<7);

        emit writeDevSignal(hardWareAddress, 21 , QString::number(data,16), false);
        Analog_lineEdit[0].setText("");
        Analog_lineEdit[1].setText("");
        Analog_lineEdit[2].setText("");
        Analog_lineEdit[3].setText("");
        Analog_lineEdit[4].setText("");
        Analog_lineEdit[5].setText("");
        break;
    }case 2:{
        int data = Analog_lineEdit[0].text().toInt(NULL,16) + (Analog_lineEdit[1].text().toInt(NULL,16)<<3) +
                   (Analog_lineEdit[2].text().toInt(NULL,16)<<4) + (Analog_lineEdit[3].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[4].text().toInt(NULL,16)<<6) + (Analog_lineEdit[5].text().toInt(NULL,16)<<7);

        emit writeDevSignal(hardWareAddress, 21 , QString::number(data,16), false);
        Analog_lineEdit[0].setText("");
        Analog_lineEdit[1].setText("");
        Analog_lineEdit[2].setText("");
        Analog_lineEdit[3].setText("");
        Analog_lineEdit[4].setText("");
        Analog_lineEdit[5].setText("");
        break;
    }case 3:{
        int data = Analog_lineEdit[0].text().toInt(NULL,16) + (Analog_lineEdit[1].text().toInt(NULL,16)<<3) +
                   (Analog_lineEdit[2].text().toInt(NULL,16)<<4) + (Analog_lineEdit[3].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[4].text().toInt(NULL,16)<<6) + (Analog_lineEdit[5].text().toInt(NULL,16)<<7);

        emit writeDevSignal(hardWareAddress, 21 , QString::number(data,16), false);
        Analog_lineEdit[0].setText("");
        Analog_lineEdit[1].setText("");
        Analog_lineEdit[2].setText("");
        Analog_lineEdit[3].setText("");
        Analog_lineEdit[4].setText("");
        Analog_lineEdit[5].setText("");
        break;
    }case 4:{
        int data = Analog_lineEdit[0].text().toInt(NULL,16) + (Analog_lineEdit[1].text().toInt(NULL,16)<<3) +
                   (Analog_lineEdit[2].text().toInt(NULL,16)<<4) + (Analog_lineEdit[3].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[4].text().toInt(NULL,16)<<6) + (Analog_lineEdit[5].text().toInt(NULL,16)<<7);

        emit writeDevSignal(hardWareAddress, 21 , QString::number(data,16), false);
        Analog_lineEdit[0].setText("");
        Analog_lineEdit[1].setText("");
        Analog_lineEdit[2].setText("");
        Analog_lineEdit[3].setText("");
        Analog_lineEdit[4].setText("");
        Analog_lineEdit[5].setText("");
        break;
    }case 5:{
        int data = Analog_lineEdit[0].text().toInt(NULL,16) + (Analog_lineEdit[1].text().toInt(NULL,16)<<3) +
                   (Analog_lineEdit[2].text().toInt(NULL,16)<<4) + (Analog_lineEdit[3].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[4].text().toInt(NULL,16)<<6) + (Analog_lineEdit[5].text().toInt(NULL,16)<<7);

        emit writeDevSignal(hardWareAddress, 21 , QString::number(data,16), false);
        Analog_lineEdit[0].setText("");
        Analog_lineEdit[1].setText("");
        Analog_lineEdit[2].setText("");
        Analog_lineEdit[3].setText("");
        Analog_lineEdit[4].setText("");
        Analog_lineEdit[5].setText("");
        break;
    }case 6:{
        int data = Analog_lineEdit[6].text().toInt(NULL,16) + (Analog_lineEdit[7].text().toInt(NULL,16)<<6);
        emit writeDevSignal(hardWareAddress, 22 , QString::number(data,16), false);
        Analog_lineEdit[6].setText("");
        Analog_lineEdit[7].setText("");
        break;
    }case 7:{
        int data = Analog_lineEdit[6].text().toInt(NULL,16) + (Analog_lineEdit[7].text().toInt(NULL,16)<<6);
        emit writeDevSignal(hardWareAddress, 22 , QString::number(data,16), false);
        Analog_lineEdit[6].setText("");
        Analog_lineEdit[7].setText("");
        break;
    }case 8:{
        int data = Analog_lineEdit[8].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 23 , QString::number(data,16), false);
        Analog_lineEdit[8].setText("");
        break;
    }case 9:{
        int data = (Analog_lineEdit[9].text().toInt(NULL,16)<<7) + (Analog_lineEdit[10].text().toInt(NULL,16)<<6) +
                   (Analog_lineEdit[11].text().toInt(NULL,16)<<5) + Analog_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 24 , QString::number(data,16), false);
        Analog_lineEdit[9].setText("");
        Analog_lineEdit[10].setText("");
        Analog_lineEdit[11].setText("");
        Analog_lineEdit[12].setText("");
        break;
    }case 10:{
        int data = (Analog_lineEdit[9].text().toInt(NULL,16)<<7) + (Analog_lineEdit[10].text().toInt(NULL,16)<<6) +
                   (Analog_lineEdit[11].text().toInt(NULL,16)<<5) + Analog_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 24 , QString::number(data,16), false);
        Analog_lineEdit[9].setText("");
        Analog_lineEdit[10].setText("");
        Analog_lineEdit[11].setText("");
        Analog_lineEdit[12].setText("");
        break;
    }case 11:{
        int data = (Analog_lineEdit[9].text().toInt(NULL,16)<<7) + (Analog_lineEdit[10].text().toInt(NULL,16)<<6) +
                   (Analog_lineEdit[11].text().toInt(NULL,16)<<5) + Analog_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 24 , QString::number(data,16), false);
        Analog_lineEdit[9].setText("");
        Analog_lineEdit[10].setText("");
        Analog_lineEdit[11].setText("");
        Analog_lineEdit[12].setText("");
        break;
    }case 12:{
        int data = (Analog_lineEdit[9].text().toInt(NULL,16)<<7) + (Analog_lineEdit[10].text().toInt(NULL,16)<<6) +
                   (Analog_lineEdit[11].text().toInt(NULL,16)<<5) + Analog_lineEdit[12].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 24 , QString::number(data,16), false);
        Analog_lineEdit[9].setText("");
        Analog_lineEdit[10].setText("");
        Analog_lineEdit[11].setText("");
        Analog_lineEdit[12].setText("");
        break;
    }case 13:{
        int data = Analog_lineEdit[13].text().toInt(NULL,16) + (Analog_lineEdit[14].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[15].text().toInt(NULL,16)<<4) + (Analog_lineEdit[16].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 25 , QString::number(data,16), false);
        Analog_lineEdit[13].setText("");
        Analog_lineEdit[14].setText("");
        Analog_lineEdit[15].setText("");
        Analog_lineEdit[16].setText("");
        break;
    }case 14:{
        int data = Analog_lineEdit[13].text().toInt(NULL,16) + (Analog_lineEdit[14].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[15].text().toInt(NULL,16)<<4) + (Analog_lineEdit[16].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 25 , QString::number(data,16), false);
        Analog_lineEdit[13].setText("");
        Analog_lineEdit[14].setText("");
        Analog_lineEdit[15].setText("");
        Analog_lineEdit[16].setText("");
        break;
    }case 15:{
        int data = Analog_lineEdit[13].text().toInt(NULL,16) + (Analog_lineEdit[14].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[15].text().toInt(NULL,16)<<4) + (Analog_lineEdit[16].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 25 , QString::number(data,16), false);
        Analog_lineEdit[13].setText("");
        Analog_lineEdit[14].setText("");
        Analog_lineEdit[15].setText("");
        Analog_lineEdit[16].setText("");
        break;
    }case 16:{
        int data = Analog_lineEdit[13].text().toInt(NULL,16) + (Analog_lineEdit[14].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[15].text().toInt(NULL,16)<<4) + (Analog_lineEdit[16].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 25 , QString::number(data,16), false);
        Analog_lineEdit[13].setText("");
        Analog_lineEdit[14].setText("");
        Analog_lineEdit[15].setText("");
        Analog_lineEdit[16].setText("");
        break;
    }case 17:{
        int data = Analog_lineEdit[17].text().toInt(NULL,16) + (Analog_lineEdit[18].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[19].text().toInt(NULL,16)<<5) + (Analog_lineEdit[20].text().toInt(NULL,16)<<6);
        emit writeDevSignal(hardWareAddress, 26 , QString::number(data,16), false);
        Analog_lineEdit[17].setText("");
        Analog_lineEdit[18].setText("");
        Analog_lineEdit[19].setText("");
        Analog_lineEdit[20].setText("");
        break;
    }case 18:{
        int data = Analog_lineEdit[17].text().toInt(NULL,16) + (Analog_lineEdit[18].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[19].text().toInt(NULL,16)<<5) + (Analog_lineEdit[20].text().toInt(NULL,16)<<6);
        emit writeDevSignal(hardWareAddress, 26 , QString::number(data,16), false);
        Analog_lineEdit[17].setText("");
        Analog_lineEdit[18].setText("");
        Analog_lineEdit[19].setText("");
        Analog_lineEdit[20].setText("");
        break;
    }case 19:{
        int data = Analog_lineEdit[17].text().toInt(NULL,16) + (Analog_lineEdit[18].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[19].text().toInt(NULL,16)<<5) + (Analog_lineEdit[20].text().toInt(NULL,16)<<6);
        emit writeDevSignal(hardWareAddress, 26 , QString::number(data,16), false);
        Analog_lineEdit[17].setText("");
        Analog_lineEdit[18].setText("");
        Analog_lineEdit[19].setText("");
        Analog_lineEdit[20].setText("");
        break;
    }case 20:{
        int data = Analog_lineEdit[17].text().toInt(NULL,16) + (Analog_lineEdit[18].text().toInt(NULL,16)<<1) +
                   (Analog_lineEdit[19].text().toInt(NULL,16)<<5) + (Analog_lineEdit[20].text().toInt(NULL,16)<<6);
        emit writeDevSignal(hardWareAddress, 26 , QString::number(data,16), false);
        Analog_lineEdit[17].setText("");
        Analog_lineEdit[18].setText("");
        Analog_lineEdit[19].setText("");
        Analog_lineEdit[20].setText("");
        break;
    }case 21:{
        int data = Analog_lineEdit[21].text().toInt(NULL,16) + (Analog_lineEdit[22].text().toInt(NULL,16)<<2) +
                   (Analog_lineEdit[23].text().toInt(NULL,16)<<4) + (Analog_lineEdit[24].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[25].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 27 , QString::number(data,16), false);
        Analog_lineEdit[21].setText("");
        Analog_lineEdit[22].setText("");
        Analog_lineEdit[23].setText("");
        Analog_lineEdit[24].setText("");
        Analog_lineEdit[25].setText("");
        break;
    }case 22:{
        int data = Analog_lineEdit[21].text().toInt(NULL,16) + (Analog_lineEdit[22].text().toInt(NULL,16)<<2) +
                   (Analog_lineEdit[23].text().toInt(NULL,16)<<4) + (Analog_lineEdit[24].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[25].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 27 , QString::number(data,16), false);
        Analog_lineEdit[21].setText("");
        Analog_lineEdit[22].setText("");
        Analog_lineEdit[23].setText("");
        Analog_lineEdit[24].setText("");
        Analog_lineEdit[25].setText("");
        break;
    }case 23:{
        int data = Analog_lineEdit[21].text().toInt(NULL,16) + (Analog_lineEdit[22].text().toInt(NULL,16)<<2) +
                   (Analog_lineEdit[23].text().toInt(NULL,16)<<4) + (Analog_lineEdit[24].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[25].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 27 , QString::number(data,16), false);
        Analog_lineEdit[21].setText("");
        Analog_lineEdit[22].setText("");
        Analog_lineEdit[23].setText("");
        Analog_lineEdit[24].setText("");
        Analog_lineEdit[25].setText("");
        break;
    }case 24:{
        int data = Analog_lineEdit[21].text().toInt(NULL,16) + (Analog_lineEdit[22].text().toInt(NULL,16)<<2) +
                   (Analog_lineEdit[23].text().toInt(NULL,16)<<4) + (Analog_lineEdit[24].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[25].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 27 , QString::number(data,16), false);
        Analog_lineEdit[21].setText("");
        Analog_lineEdit[22].setText("");
        Analog_lineEdit[23].setText("");
        Analog_lineEdit[24].setText("");
        Analog_lineEdit[25].setText("");
        break;
    }case 25:{
        int data = Analog_lineEdit[21].text().toInt(NULL,16) + (Analog_lineEdit[22].text().toInt(NULL,16)<<2) +
                   (Analog_lineEdit[23].text().toInt(NULL,16)<<4) + (Analog_lineEdit[24].text().toInt(NULL,16)<<5) +
                   (Analog_lineEdit[25].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 27 , QString::number(data,16), false);
        Analog_lineEdit[21].setText("");
        Analog_lineEdit[22].setText("");
        Analog_lineEdit[23].setText("");
        Analog_lineEdit[24].setText("");
        Analog_lineEdit[25].setText("");
        break;
    }
    default:
        break;
    }
}


//读取Pixel槽函数
void MainWindow::Pixel_read_slot(int Pixel_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read Pixel_number = "<<Pixel_number<<endl;
    int hardWareAddress = 216;

    switch (Pixel_number) {
    case 0:
        emit readDevSignal(hardWareAddress,28,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,28,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,29,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,29,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,29,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,29,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,29,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,30,false);
        break;
    case 8:
        emit readDevSignal(hardWareAddress,30,false);
        break;
    case 9:
        emit readDevSignal(hardWareAddress,31,false);
        break;
    case 10:
        emit readDevSignal(hardWareAddress,31,false);
        break;
    case 11:
        emit readDevSignal(hardWareAddress,32,false);
        break;
    case 12:
        emit readDevSignal(hardWareAddress,32,false);
        break;
    case 13:
        emit readDevSignal(hardWareAddress,32,false);
        break;
    case 14:
        emit readDevSignal(hardWareAddress,32,false);
        break;
    case 15:
        emit readDevSignal(hardWareAddress,32,false);
        break;
    case 16:
        emit readDevSignal(hardWareAddress,32,false);
        break;
    default:
        break;

    }




}
//写入Pixel槽函数
void MainWindow::Pixel_write_slot(int Pixel_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write Pixel_number = "<<Pixel_number<<endl;
    int hardWareAddress = 216;
    switch (Pixel_number) {
    case 0:{
        int data = (Pixel_lineEdit[0].text().toInt(NULL,16)<<4) + Pixel_lineEdit[1].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 28 , QString::number(data,16), false);
        Pixel_lineEdit[0].setText("");
        Pixel_lineEdit[1].setText("");
        break;
    }case 1:{
        int data = (Pixel_lineEdit[0].text().toInt(NULL,16)<<4) + Pixel_lineEdit[1].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 28 , QString::number(data,16), false);
        Pixel_lineEdit[0].setText("");
        Pixel_lineEdit[1].setText("");
        break;
    }case 2:{
        int data = (Pixel_lineEdit[2].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[3].text().toInt(NULL,16)<<3) +
                   (Pixel_lineEdit[4].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[5].text().toInt(NULL,16)<<1) +
                    Pixel_lineEdit[6].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 29 , QString::number(data,16), false);
        Pixel_lineEdit[2].setText("");
        Pixel_lineEdit[3].setText("");
        Pixel_lineEdit[4].setText("");
        Pixel_lineEdit[5].setText("");
        Pixel_lineEdit[6].setText("");
        break;
    }case 3:{
        int data = (Pixel_lineEdit[2].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[3].text().toInt(NULL,16)<<3) +
                   (Pixel_lineEdit[4].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[5].text().toInt(NULL,16)<<1) +
                    Pixel_lineEdit[6].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 29 , QString::number(data,16), false);
        Pixel_lineEdit[2].setText("");
        Pixel_lineEdit[3].setText("");
        Pixel_lineEdit[4].setText("");
        Pixel_lineEdit[5].setText("");
        Pixel_lineEdit[6].setText("");
        break;
    }case 4:{
        int data = (Pixel_lineEdit[2].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[3].text().toInt(NULL,16)<<3) +
                   (Pixel_lineEdit[4].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[5].text().toInt(NULL,16)<<1) +
                    Pixel_lineEdit[6].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 29 , QString::number(data,16), false);
        Pixel_lineEdit[2].setText("");
        Pixel_lineEdit[3].setText("");
        Pixel_lineEdit[4].setText("");
        Pixel_lineEdit[5].setText("");
        Pixel_lineEdit[6].setText("");
        break;
    }case 5:{
        int data = (Pixel_lineEdit[2].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[3].text().toInt(NULL,16)<<3) +
                   (Pixel_lineEdit[4].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[5].text().toInt(NULL,16)<<1) +
                    Pixel_lineEdit[6].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 29 , QString::number(data,16), false);
        Pixel_lineEdit[2].setText("");
        Pixel_lineEdit[3].setText("");
        Pixel_lineEdit[4].setText("");
        Pixel_lineEdit[5].setText("");
        Pixel_lineEdit[6].setText("");
        break;
    }case 6:{
        int data = (Pixel_lineEdit[2].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[3].text().toInt(NULL,16)<<3) +
                   (Pixel_lineEdit[4].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[5].text().toInt(NULL,16)<<1) +
                    Pixel_lineEdit[6].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 29 , QString::number(data,16), false);
        Pixel_lineEdit[2].setText("");
        Pixel_lineEdit[3].setText("");
        Pixel_lineEdit[4].setText("");
        Pixel_lineEdit[5].setText("");
        Pixel_lineEdit[6].setText("");
        break;
    }case 7:{
        int data = (Pixel_lineEdit[7].text().toInt(NULL,16)<<4) + Pixel_lineEdit[8].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 30 , QString::number(data,16), false);
        Pixel_lineEdit[7].setText("");
        Pixel_lineEdit[8].setText("");
        break;
    }case 8:{
        int data = (Pixel_lineEdit[7].text().toInt(NULL,16)<<4) + Pixel_lineEdit[8].text().toInt(NULL,16) ;
        emit writeDevSignal(hardWareAddress, 30 , QString::number(data,16), false);
        Pixel_lineEdit[7].setText("");
        Pixel_lineEdit[8].setText("");
        break;
    }case 9:{
        int data = Pixel_lineEdit[9].text().toInt(NULL,16) + (Pixel_lineEdit[10].text().toInt(NULL,16)<<5) ;
        emit writeDevSignal(hardWareAddress, 31 , QString::number(data,16), false);
        Pixel_lineEdit[9].setText("");
        Pixel_lineEdit[10].setText("");
        break;
    }case 10:{
        int data = Pixel_lineEdit[9].text().toInt(NULL,16) + (Pixel_lineEdit[10].text().toInt(NULL,16)<<5) ;
        emit writeDevSignal(hardWareAddress, 31 , QString::number(data,16), false);
        Pixel_lineEdit[9].setText("");
        Pixel_lineEdit[10].setText("");
        break;
    }case 11:{
        int data = Pixel_lineEdit[11].text().toInt(NULL,16) + (Pixel_lineEdit[12].text().toInt(NULL,16)<<1) +
                  (Pixel_lineEdit[13].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[14].text().toInt(NULL,16)<<3) +
                  (Pixel_lineEdit[15].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[16].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 32 , QString::number(data,16), false);
        Pixel_lineEdit[11].setText("");
        Pixel_lineEdit[12].setText("");
        Pixel_lineEdit[13].setText("");
        Pixel_lineEdit[14].setText("");
        Pixel_lineEdit[15].setText("");
        Pixel_lineEdit[16].setText("");
        break;
    }case 12:{
        int data = Pixel_lineEdit[11].text().toInt(NULL,16) + (Pixel_lineEdit[12].text().toInt(NULL,16)<<1) +
                  (Pixel_lineEdit[13].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[14].text().toInt(NULL,16)<<3) +
                  (Pixel_lineEdit[15].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[16].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 32 , QString::number(data,16), false);
        Pixel_lineEdit[11].setText("");
        Pixel_lineEdit[12].setText("");
        Pixel_lineEdit[13].setText("");
        Pixel_lineEdit[14].setText("");
        Pixel_lineEdit[15].setText("");
        Pixel_lineEdit[16].setText("");
        break;
    }case 13:{
        int data = Pixel_lineEdit[11].text().toInt(NULL,16) + (Pixel_lineEdit[12].text().toInt(NULL,16)<<1) +
                  (Pixel_lineEdit[13].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[14].text().toInt(NULL,16)<<3) +
                  (Pixel_lineEdit[15].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[16].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 32 , QString::number(data,16), false);
        Pixel_lineEdit[11].setText("");
        Pixel_lineEdit[12].setText("");
        Pixel_lineEdit[13].setText("");
        Pixel_lineEdit[14].setText("");
        Pixel_lineEdit[15].setText("");
        Pixel_lineEdit[16].setText("");
        break;
    }case 14:{
        int data = Pixel_lineEdit[11].text().toInt(NULL,16) + (Pixel_lineEdit[12].text().toInt(NULL,16)<<1) +
                  (Pixel_lineEdit[13].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[14].text().toInt(NULL,16)<<3) +
                  (Pixel_lineEdit[15].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[16].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 32 , QString::number(data,16), false);
        Pixel_lineEdit[11].setText("");
        Pixel_lineEdit[12].setText("");
        Pixel_lineEdit[13].setText("");
        Pixel_lineEdit[14].setText("");
        Pixel_lineEdit[15].setText("");
        Pixel_lineEdit[16].setText("");
        break;
    }case 15:{
        int data = Pixel_lineEdit[11].text().toInt(NULL,16) + (Pixel_lineEdit[12].text().toInt(NULL,16)<<1) +
                  (Pixel_lineEdit[13].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[14].text().toInt(NULL,16)<<3) +
                  (Pixel_lineEdit[15].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[16].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 32 , QString::number(data,16), false);
        Pixel_lineEdit[11].setText("");
        Pixel_lineEdit[12].setText("");
        Pixel_lineEdit[13].setText("");
        Pixel_lineEdit[14].setText("");
        Pixel_lineEdit[15].setText("");
        Pixel_lineEdit[16].setText("");
        break;
    }case 16:{
        int data = Pixel_lineEdit[11].text().toInt(NULL,16) + (Pixel_lineEdit[12].text().toInt(NULL,16)<<1) +
                  (Pixel_lineEdit[13].text().toInt(NULL,16)<<2) + (Pixel_lineEdit[14].text().toInt(NULL,16)<<3) +
                  (Pixel_lineEdit[15].text().toInt(NULL,16)<<4) + (Pixel_lineEdit[16].text().toInt(NULL,16)<<7);
        emit writeDevSignal(hardWareAddress, 32 , QString::number(data,16), false);
        Pixel_lineEdit[11].setText("");
        Pixel_lineEdit[12].setText("");
        Pixel_lineEdit[13].setText("");
        Pixel_lineEdit[14].setText("");
        Pixel_lineEdit[15].setText("");
        Pixel_lineEdit[16].setText("");
        break;
    }default:
        break;


    }



}


//读取Top槽函数
void MainWindow::Top_read_slot(int Top_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read Top_number = "<<Top_number<<endl;

    int hardWareAddress = 216;

    switch (Top_number) {
    case 0:
        emit readDevSignal(hardWareAddress,33,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,33,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,33,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,34,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,34,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,34,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,35,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,35,false);
        break;
    case 8:
        emit readDevSignal(hardWareAddress,36,false);
        break;
    case 9:
        emit readDevSignal(hardWareAddress,36,false);
        break;
    case 10:
        emit readDevSignal(hardWareAddress,36,false);
        break;
    case 11:
        emit readDevSignal(hardWareAddress,36,false);
        break;
    case 12:
        emit readDevSignal(hardWareAddress,36,false);
        break;
    }



}
//寫入Top槽函数
void MainWindow::Top_write_slot(int Top_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write Top_number = "<<Top_number<<endl;
    int hardWareAddress = 216;
    switch (Top_number) {
    case 0:{
        int data = Top_lineEdit[0].text().toInt(NULL,16) + (Top_lineEdit[1].text().toInt(NULL,16)<<4)+
                   (Top_lineEdit[2].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 33 , QString::number(data,16), false);
        Top_lineEdit[0].setText("");
        Top_lineEdit[1].setText("");
        Top_lineEdit[2].setText("");
        break;
    }case 1:{
        int data = Top_lineEdit[0].text().toInt(NULL,16) + (Top_lineEdit[1].text().toInt(NULL,16)<<4)+
                   (Top_lineEdit[2].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 33 , QString::number(data,16), false);
        Top_lineEdit[0].setText("");
        Top_lineEdit[1].setText("");
        Top_lineEdit[2].setText("");
        break;
    }case 2:{
        int data = Top_lineEdit[0].text().toInt(NULL,16) + (Top_lineEdit[1].text().toInt(NULL,16)<<4)+
                   (Top_lineEdit[2].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 33 , QString::number(data,16), false);
        Top_lineEdit[0].setText("");
        Top_lineEdit[1].setText("");
        Top_lineEdit[2].setText("");
        break;
    }case 3:{
        int data = Top_lineEdit[3].text().toInt(NULL,16) + (Top_lineEdit[4].text().toInt(NULL,16)<<1)+
                   (Top_lineEdit[5].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 34 , QString::number(data,16), false);
        Top_lineEdit[3].setText("");
        Top_lineEdit[4].setText("");
        Top_lineEdit[5].setText("");
        break;
    }case 4:{
        int data = Top_lineEdit[3].text().toInt(NULL,16) + (Top_lineEdit[4].text().toInt(NULL,16)<<1)+
                   (Top_lineEdit[5].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 34 , QString::number(data,16), false);
        Top_lineEdit[3].setText("");
        Top_lineEdit[4].setText("");
        Top_lineEdit[5].setText("");
        break;
    }case 5:{
        int data = Top_lineEdit[3].text().toInt(NULL,16) + (Top_lineEdit[4].text().toInt(NULL,16)<<1)+
                   (Top_lineEdit[5].text().toInt(NULL,16)<<5);
        emit writeDevSignal(hardWareAddress, 34 , QString::number(data,16), false);
        Top_lineEdit[3].setText("");
        Top_lineEdit[4].setText("");
        Top_lineEdit[5].setText("");
        break;
    }case 6:{
        int data = Top_lineEdit[6].text().toInt(NULL,16) + (Top_lineEdit[7].text().toInt(NULL,16)<<3);
        emit writeDevSignal(hardWareAddress, 35 , QString::number(data,16), false);
        Top_lineEdit[6].setText("");
        Top_lineEdit[7].setText("");
        break;
    }case 7:{
        int data = Top_lineEdit[6].text().toInt(NULL,16) + (Top_lineEdit[7].text().toInt(NULL,16)<<3);
        emit writeDevSignal(hardWareAddress, 35 , QString::number(data,16), false);
        Top_lineEdit[6].setText("");
        Top_lineEdit[7].setText("");
        break;
    }case 8:{
        int data = Top_lineEdit[8].text().toInt(NULL,16) + (Top_lineEdit[9].text().toInt(NULL,16)<<1) +
                (Top_lineEdit[10].text().toInt(NULL,16)<<3) + (Top_lineEdit[11].text().toInt(NULL,16)<<5) +
                (Top_lineEdit[12].text().toInt(NULL,16)<<7) ;
        emit writeDevSignal(hardWareAddress, 36 , QString::number(data,16), false);
        Top_lineEdit[8].setText("");
        Top_lineEdit[9].setText("");
        Top_lineEdit[10].setText("");
        Top_lineEdit[11].setText("");
        Top_lineEdit[12].setText("");
        break;
    }case 9:{
        int data = Top_lineEdit[8].text().toInt(NULL,16) + (Top_lineEdit[9].text().toInt(NULL,16)<<1) +
                (Top_lineEdit[10].text().toInt(NULL,16)<<3) + (Top_lineEdit[11].text().toInt(NULL,16)<<5) +
                (Top_lineEdit[12].text().toInt(NULL,16)<<7) ;
        emit writeDevSignal(hardWareAddress, 36 , QString::number(data,16), false);
        Top_lineEdit[8].setText("");
        Top_lineEdit[9].setText("");
        Top_lineEdit[10].setText("");
        Top_lineEdit[11].setText("");
        Top_lineEdit[12].setText("");
        break;
    }case 10:{
        int data = Top_lineEdit[8].text().toInt(NULL,16) + (Top_lineEdit[9].text().toInt(NULL,16)<<1) +
                (Top_lineEdit[10].text().toInt(NULL,16)<<3) + (Top_lineEdit[11].text().toInt(NULL,16)<<5) +
                (Top_lineEdit[12].text().toInt(NULL,16)<<7) ;
        emit writeDevSignal(hardWareAddress, 36 , QString::number(data,16), false);
        Top_lineEdit[8].setText("");
        Top_lineEdit[9].setText("");
        Top_lineEdit[10].setText("");
        Top_lineEdit[11].setText("");
        Top_lineEdit[12].setText("");
        break;
    }case 11:{
        int data = Top_lineEdit[8].text().toInt(NULL,16) + (Top_lineEdit[9].text().toInt(NULL,16)<<1) +
                (Top_lineEdit[10].text().toInt(NULL,16)<<3) + (Top_lineEdit[11].text().toInt(NULL,16)<<5) +
                (Top_lineEdit[12].text().toInt(NULL,16)<<7) ;
        emit writeDevSignal(hardWareAddress, 36 , QString::number(data,16), false);
        Top_lineEdit[8].setText("");
        Top_lineEdit[9].setText("");
        Top_lineEdit[10].setText("");
        Top_lineEdit[11].setText("");
        Top_lineEdit[12].setText("");
        break;
    }case 12:{
        int data = Top_lineEdit[8].text().toInt(NULL,16) + (Top_lineEdit[9].text().toInt(NULL,16)<<1) +
                (Top_lineEdit[10].text().toInt(NULL,16)<<3) + (Top_lineEdit[11].text().toInt(NULL,16)<<5) +
                (Top_lineEdit[12].text().toInt(NULL,16)<<7) ;
        emit writeDevSignal(hardWareAddress, 36 , QString::number(data,16), false);
        Top_lineEdit[8].setText("");
        Top_lineEdit[9].setText("");
        Top_lineEdit[10].setText("");
        Top_lineEdit[11].setText("");
        Top_lineEdit[12].setText("");
        break;
    }default:
        break;

    }






}


//读取Delayline槽函数
void MainWindow::Delayline_read_slot(int Delayline_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read Delayline_number = "<<Delayline_number<<endl;

    int hardWareAddress = 216;

    switch (Delayline_number) {
    case 0:
        emit readDevSignal(hardWareAddress,37,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,37,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,38,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,39,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,40,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,41,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,42,false);
        break;
     default:
        break;
    }




}
//写入Delayline槽函数
void MainWindow::Delayline_write_slot(int Delayline_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write Delayline_number = "<<Delayline_number<<endl;
    int hardWareAddress = 216;
    switch (Delayline_number) {
    case 0:{
        int data = (Delayline_lineEdit[0].text().toInt(NULL,16)<<3) + Delayline_lineEdit[1].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 37 , QString::number(data,16), false);
        Delayline_lineEdit[0].setText("");
        Delayline_lineEdit[1].setText("");
        break;
    }case 1:{
        int data = (Delayline_lineEdit[0].text().toInt(NULL,16)<<3) + Delayline_lineEdit[1].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 37 , QString::number(data,16), false);
        Delayline_lineEdit[0].setText("");
        Delayline_lineEdit[1].setText("");
        break;
    }case 2:{
        int data =  Delayline_lineEdit[2].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 38 , QString::number(data,16), false);
        Delayline_lineEdit[2].setText("");
        break;
    }case 3:{
        int data =  Delayline_lineEdit[3].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 39 , QString::number(data,16), false);
        Delayline_lineEdit[3].setText("");
        break;
    }case 4:{
        int data =  Delayline_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 40 , QString::number(data,16), false);
        Delayline_lineEdit[4].setText("");
        break;
    }case 5:{
        int data =  Delayline_lineEdit[5].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 41 , QString::number(data,16), false);
        Delayline_lineEdit[5].setText("");
        break;
    }case 6:{
        int data =  Delayline_lineEdit[6].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 42 , QString::number(data,16), false);
        Delayline_lineEdit[6].setText("");
        break;
    }

    }



}


//读取MISC槽函数
void MainWindow::MISC_read_slot(int MISC_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read MISC_number = "<<MISC_number<<endl;
    int hardWareAddress = 216;

    switch (MISC_number) {
    case 0:
        emit readDevSignal(hardWareAddress,43,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,43,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,43,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,43,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,44,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,45,false);
        break;
    case 6:
        emit readDevSignal(hardWareAddress,46,false);
        break;
    case 7:
        emit readDevSignal(hardWareAddress,47,false);
        break;
     default:
        break;
    }
}
//写入MISC槽函数
void MainWindow::MISC_write_slot(int MISC_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write MISC_number = "<<MISC_number<<endl;
    int hardWareAddress = 216;
    switch (MISC_number) {
    case 0:{
        int data = (MISC_lineEdit[0].text().toInt(NULL,16)<<7) + (MISC_lineEdit[1].text().toInt(NULL,16)<<4) +
                (MISC_lineEdit[2].text().toInt(NULL,16)<<3) + MISC_lineEdit[3].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 43 , QString::number(data,16), false);
        MISC_lineEdit[0].setText("");
        MISC_lineEdit[1].setText("");
        MISC_lineEdit[2].setText("");
        MISC_lineEdit[3].setText("");
        break;
    }case 1:{
        int data = (MISC_lineEdit[0].text().toInt(NULL,16)<<7) + (MISC_lineEdit[1].text().toInt(NULL,16)<<4) +
                (MISC_lineEdit[2].text().toInt(NULL,16)<<3) + MISC_lineEdit[3].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 43 , QString::number(data,16), false);
        MISC_lineEdit[0].setText("");
        MISC_lineEdit[1].setText("");
        MISC_lineEdit[2].setText("");
        MISC_lineEdit[3].setText("");
        break;
    }case 2:{
        int data = (MISC_lineEdit[0].text().toInt(NULL,16)<<7) + (MISC_lineEdit[1].text().toInt(NULL,16)<<4) +
                (MISC_lineEdit[2].text().toInt(NULL,16)<<3) + MISC_lineEdit[3].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 43 , QString::number(data,16), false);
        MISC_lineEdit[0].setText("");
        MISC_lineEdit[1].setText("");
        MISC_lineEdit[2].setText("");
        MISC_lineEdit[3].setText("");
        break;
        break;
    }case 3:{
        int data = (MISC_lineEdit[0].text().toInt(NULL,16)<<7) + (MISC_lineEdit[1].text().toInt(NULL,16)<<4) +
                (MISC_lineEdit[2].text().toInt(NULL,16)<<3) + MISC_lineEdit[3].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 43 , QString::number(data,16), false);
        MISC_lineEdit[0].setText("");
        MISC_lineEdit[1].setText("");
        MISC_lineEdit[2].setText("");
        MISC_lineEdit[3].setText("");
        break;
        break;
    }case 4:{
        int data =  MISC_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 44 , QString::number(data,16), false);
        MISC_lineEdit[4].setText("");
        break;
    }case 5:{
        int data =  MISC_lineEdit[5].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 45 , QString::number(data,16), false);
        MISC_lineEdit[5].setText("");
        break;
    }case 6:{
        int data =  MISC_lineEdit[6].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 46 , QString::number(data,16), false);
        MISC_lineEdit[6].setText("");
        break;
    }case 7:{
        int data =  MISC_lineEdit[7].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 47 , QString::number(data,16), false);
        MISC_lineEdit[7].setText("");
        break;
    }

    }
}


//读取Others槽函数
void MainWindow::Others_read_slot(int Others_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"read Others_number = "<<Others_number<<endl;

    int hardWareAddress = 216;

    switch (Others_number) {
    case 0:
        emit readDevSignal(hardWareAddress,48,false);
        break;
    case 1:
        emit readDevSignal(hardWareAddress,48,false);
        break;
    case 2:
        emit readDevSignal(hardWareAddress,48,false);
        break;
    case 3:
        emit readDevSignal(hardWareAddress,48,false);
        break;
    case 4:
        emit readDevSignal(hardWareAddress,48,false);
        break;
    case 5:
        emit readDevSignal(hardWareAddress,49,false);
        break;
     default:
        break;
    }

}
//写入Others槽函数
void MainWindow::Others_write_slot(int Others_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write Others_number = "<<Others_number<<endl;
    int hardWareAddress = 216;
    switch (Others_number) {
    case 0:{
        int data = (Others_lineEdit[0].text().toInt(NULL,16)<<7) + (Others_lineEdit[1].text().toInt(NULL,16)<<6) +
                (Others_lineEdit[2].text().toInt(NULL,16)<<5) + (Others_lineEdit[3].text().toInt(NULL,16)<<4) +
                Others_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 48 , QString::number(data,16), false);
        Others_lineEdit[0].setText("");
        Others_lineEdit[1].setText("");
        Others_lineEdit[2].setText("");
        Others_lineEdit[3].setText("");
        Others_lineEdit[4].setText("");
        break;
    }case 1:{
        int data = (Others_lineEdit[0].text().toInt(NULL,16)<<7) + (Others_lineEdit[1].text().toInt(NULL,16)<<6) +
                (Others_lineEdit[2].text().toInt(NULL,16)<<5) + (Others_lineEdit[3].text().toInt(NULL,16)<<4) +
                Others_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 48 , QString::number(data,16), false);
        Others_lineEdit[0].setText("");
        Others_lineEdit[1].setText("");
        Others_lineEdit[2].setText("");
        Others_lineEdit[3].setText("");
        Others_lineEdit[4].setText("");
        break;
    }case 2:{
        int data = (Others_lineEdit[0].text().toInt(NULL,16)<<7) + (Others_lineEdit[1].text().toInt(NULL,16)<<6) +
                (Others_lineEdit[2].text().toInt(NULL,16)<<5) + (Others_lineEdit[3].text().toInt(NULL,16)<<4) +
                Others_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 48 , QString::number(data,16), false);
        Others_lineEdit[0].setText("");
        Others_lineEdit[1].setText("");
        Others_lineEdit[2].setText("");
        Others_lineEdit[3].setText("");
        Others_lineEdit[4].setText("");
        break;
    }case 3:{
        int data = (Others_lineEdit[0].text().toInt(NULL,16)<<7) + (Others_lineEdit[1].text().toInt(NULL,16)<<6) +
                (Others_lineEdit[2].text().toInt(NULL,16)<<5) + (Others_lineEdit[3].text().toInt(NULL,16)<<4) +
                Others_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 48 , QString::number(data,16), false);
        Others_lineEdit[0].setText("");
        Others_lineEdit[1].setText("");
        Others_lineEdit[2].setText("");
        Others_lineEdit[3].setText("");
        Others_lineEdit[4].setText("");
        break;
    }case 4:{
        int data = (Others_lineEdit[0].text().toInt(NULL,16)<<7) + (Others_lineEdit[1].text().toInt(NULL,16)<<6) +
                (Others_lineEdit[2].text().toInt(NULL,16)<<5) + (Others_lineEdit[3].text().toInt(NULL,16)<<4) +
                Others_lineEdit[4].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 48 , QString::number(data,16), false);
        Others_lineEdit[0].setText("");
        Others_lineEdit[1].setText("");
        Others_lineEdit[2].setText("");
        Others_lineEdit[3].setText("");
        Others_lineEdit[4].setText("");
        break;
    }case 5:{
        int data = Others_lineEdit[5].text().toInt(NULL,16);
        emit writeDevSignal(hardWareAddress, 49 , QString::number(data,16), false);
        Others_lineEdit[5].setText("");
        break;
    }

}



}


/*******************统一接收寄存器信号的 槽函数 **************************/

//读取设备指令返回信息的槽函数
//对于读取寄存器 address registerAddress data 都是十进制
void MainWindow::reReadDevSlot(int regesiterAddress,QString str)
{
    qDebug()<<"regesiterAddress = "<<regesiterAddress<<"  str="<<str<<endl;

    quint8 data =  quint8(str.toInt());
    switch (regesiterAddress) {
    case 0:{
        int sfw_rst = data & 0x01;
        TDC_lineEdit[0].setText(QString::number(sfw_rst,16).toUpper());
        break;
    }case 1:{
        int r_cnt_rst_dly1 = (data & 0xF0)>>4;
        TDC_lineEdit[1].setText(QString::number(r_cnt_rst_dly1,16).toUpper());
        int r_syncnt_rst_width = data & 0x0F;
        TDC_lineEdit[2].setText(QString::number(r_syncnt_rst_width,16).toUpper());
        break;
    }case 2:{
        int r_cnt_hld_dly2 = data;
        TDC_lineEdit[3].setText(QString::number(r_cnt_hld_dly2,16).toUpper());
        break;
    }case 3:{
        int r_tdc_rdck_dly1 = (data & 0xF0)>>4;
        TDC_lineEdit[4].setText(QString::number(r_tdc_rdck_dly1,16).toUpper());
        int r_tdc_redn_dly = data & 0x0F;
        TDC_lineEdit[5].setText(QString::number(r_tdc_redn_dly,16).toUpper());
        break;
    }case 4:{
        int r_tdc_cnt_rst_dly2 = (data & 0xF0)>>4;
        TDC_lineEdit[6].setText(QString::number(r_tdc_cnt_rst_dly2,16).toUpper());
        int r_tdc_rdck_cyc = data & 0x0F;
        TDC_lineEdit[7].setText(QString::number(r_tdc_rdck_cyc,16).toUpper());
        break;
    }case 5:{
        int r_rising_latch = (data & 0x80)>>7;
        TDC_lineEdit[8].setText(QString::number(r_rising_latch,16).toUpper());
        int r_tdc_read_en_same = (data & 0x40)>>6;
        TDC_lineEdit[9].setText(QString::number(r_tdc_read_en_same,16).toUpper());
        int r_slower_clk = (data & 0x20)>>5;
        TDC_lineEdit[10].setText(QString::number(r_slower_clk,16).toUpper());
        int r_faster_clk = (data & 0x10)>>4;
        TDC_lineEdit[11].setText(QString::number(r_faster_clk,16).toUpper());
        int r_cnt_hld_dly1 = data & 0x0F;
        TDC_lineEdit[12].setText(QString::number(r_cnt_hld_dly1,16).toUpper());
        break;
    }case 6:{
        int r_integ = data;
        Integration_lineEdit[0].setText(QString::number(r_integ,16).toUpper());
        break;
    }case 7:{
        int r_hts = (data & 0x30)>>4;
        Integration_lineEdit[1].setText(QString::number(r_hts,16).toUpper());
        int r_integ = data & 0x0F;
        Integration_lineEdit[2].setText(QString::number(r_integ,16).toUpper());
        break;
    }case 8:{
        int r_hts = data;
        Integration_lineEdit[3].setText(QString::number(r_hts,16).toUpper());
        break;
    }case 9:{
        int r_ma_w0 = data & 0x0F;
        MA_lineEdit[0].setText(QString::number(r_ma_w0,16).toUpper());
        int r_ma_w1 = (data & 0xF0)>>4;
        MA_lineEdit[1].setText(QString::number(r_ma_w1,16).toUpper());
        break;
    }case 10:{
        int r_ma_w2 = data & 0x0F;
        MA_lineEdit[2].setText(QString::number(r_ma_w2,16).toUpper());
        int r_ma_w3 = (data & 0xF0)>>4;
        MA_lineEdit[3].setText(QString::number(r_ma_w3,16).toUpper());
        break;
    }case 11:{
        int r_ma_w4 = data & 0x0F;
        MA_lineEdit[4].setText(QString::number(r_ma_w4,16).toUpper());
        int r_ma_w5 = (data & 0xF0)>>4;
        MA_lineEdit[5].setText(QString::number(r_ma_w5,16).toUpper());
        break;
    }case 12:{
        int r_ma_w6 = data & 0x0F;
        MA_lineEdit[6].setText(QString::number(r_ma_w6,16).toUpper());
        int r_ma_w7 = (data & 0xF0)>>4;
        MA_lineEdit[7].setText(QString::number(r_ma_w7,16).toUpper());
        break;
    }case 13:{
        int r_ma_w8 = data & 0x0F;
        MA_lineEdit[8].setText(QString::number(r_ma_w8,16).toUpper());
        int r_ma_w9 = (data & 0xF0)>>4;
        MA_lineEdit[9].setText(QString::number(r_ma_w9,16).toUpper());
        break;
    }case 14:{
        int r_ma_wa = data & 0x0F;
        MA_lineEdit[10].setText(QString::number(r_ma_wa,16).toUpper());
        int r_ma_wb = (data & 0xF0)>>4;
        MA_lineEdit[11].setText(QString::number(r_ma_wb,16).toUpper());
        break;
    }case 15:{
        int r_ma_wc = data & 0x0F;
        MA_lineEdit[12].setText(QString::number(r_ma_wc,16).toUpper());
        int r_ma_wd = (data & 0xF0)>>4;
        MA_lineEdit[13].setText(QString::number(r_ma_wd,16).toUpper());
        break;
    }case 16:{
        int r_ma_we = data & 0x0F;
        MA_lineEdit[14].setText(QString::number(r_ma_we,16).toUpper());
        int r_ma_wf = (data & 0xF0)>>4;
        MA_lineEdit[15].setText(QString::number(r_ma_wf,16).toUpper());
        break;
    }case 17:{
        int r_spi_en = (data & 0x80)>>7;
        Digital_lineEdit[0].setText(QString::number(r_spi_en,16).toUpper());
        int r_dvp_clk_sel = (data & 0x70)>>4;
        Digital_lineEdit[1].setText(QString::number(r_dvp_clk_sel,16).toUpper());
        int r_clk_divider = data & 0x0F;
        Digital_lineEdit[2].setText(QString::number(r_clk_divider,16).toUpper());
        break;
    }case 18:{
        int r_sramout_clksel = (data & 0x80)>>7;
        Digital_lineEdit[3].setText(QString::number(r_sramout_clksel,16).toUpper());
        int r_raw_out_mode = (data & 0x40)>>6;
        Digital_lineEdit[4].setText(QString::number(r_raw_out_mode,16).toUpper());
        int r_dvp_sram_output_mode = (data & 0x20)>>5;
        Digital_lineEdit[5].setText(QString::number(r_dvp_sram_output_mode,16).toUpper());
        int r_sram_output_cycles = data & 0x1F;
        Digital_lineEdit[6].setText(QString::number(r_sram_output_cycles,16).toUpper());
        break;
    }case 19:{
        int r_row_start = data & 0x1F;
        Digital_lineEdit[7].setText(QString::number(r_row_start,16).toUpper());
        int r_high_bits = (data & 0x80)>>7;
        Digital_lineEdit[8].setText(QString::number(r_high_bits,16).toUpper());
        break;
    }case 20:{
        int r_row_end = data & 0x3F;
        Digital_lineEdit[9].setText(QString::number(r_row_end,16).toUpper());
        break;
    }case 21:{
        int dl_sel_dly = data & 0x07;
        Analog_lineEdit[0].setText(QString::number(dl_sel_dly,16).toUpper());
        int dl_sel_long = (data & 0x08)>>3;
        Analog_lineEdit[1].setText(QString::number(dl_sel_long,16).toUpper());
        int dl_en = (data & 0x10)>>4;
        Analog_lineEdit[2].setText(QString::number(dl_en,16).toUpper());
        int tdc_syncnt_en_global = (data & 0x20)>>5;
        Analog_lineEdit[3].setText(QString::number(tdc_syncnt_en_global,16).toUpper());
        int tdc_ckdrv_en = (data & 0x40)>>6;
        Analog_lineEdit[4].setText(QString::number(tdc_ckdrv_en,16).toUpper());
        int sel_cnt_mode = (data & 0x80)>>7;
        Analog_lineEdit[5].setText(QString::number(sel_cnt_mode,16).toUpper());
        break;
    }case 22:{
        int pll_coarse_cnt_cksel = data & 0x1F;
        Analog_lineEdit[6].setText(QString::number(pll_coarse_cnt_cksel,16).toUpper());
        int pll_lpf_rc = (data & 0xC0)>>6;
        Analog_lineEdit[7].setText(QString::number(pll_lpf_rc,16).toUpper());
        break;
    }case 23:{
        int pll_div_ctrl = data & 0x7F;
        Analog_lineEdit[8].setText(QString::number(pll_div_ctrl,16).toUpper());
        break;
    }case 24:{
        int enb_pclk = (data & 0x80)>>7;
        Analog_lineEdit[9].setText(QString::number(enb_pclk,16).toUpper());
        int r_tdc_start_re = (data & 0x40)>>6;
        Analog_lineEdit[10].setText(QString::number(r_tdc_start_re,16).toUpper());
        int mclk_div_rst = (data & 0x20)>>5;
        Analog_lineEdit[11].setText(QString::number(mclk_div_rst,16).toUpper());
        int mclk_div_ctrl = data & 0x1F;
        Analog_lineEdit[12].setText(QString::number(mclk_div_ctrl,16).toUpper());
        break;
    }case 25:{
        int ana_reserve_out_25_1 = data & 0x01;
        Analog_lineEdit[13].setText(QString::number(ana_reserve_out_25_1,16).toUpper());
        int ana_reserve_out_25_2 = (data & 0x0E)>>1;
        Analog_lineEdit[14].setText(QString::number(ana_reserve_out_25_2,16).toUpper());
        int ana_reserve_out_25_3 = (data & 0x10)>>4;
        Analog_lineEdit[15].setText(QString::number(ana_reserve_out_25_3,16).toUpper());
        int ana_reserve_out_25_4 = (data & 0xE0)>>5;
        Analog_lineEdit[16].setText(QString::number(ana_reserve_out_25_4,16).toUpper());
        break;
    }case 26:{
        int ana_reserve_out_26_1 = data & 0x01;
        Analog_lineEdit[17].setText(QString::number(ana_reserve_out_26_1,16).toUpper());
        int ana_reserve_out_26_2 = (data & 0x1E)>>1;
        Analog_lineEdit[18].setText(QString::number(ana_reserve_out_26_2,16).toUpper());
        int ana_reserve_out_26_3 = (data & 0x20)>>5;
        Analog_lineEdit[19].setText(QString::number(ana_reserve_out_26_3,16).toUpper());
        int ana_reserve_out_26_4 = (data & 0xC0)>>6;
        Analog_lineEdit[20].setText(QString::number(ana_reserve_out_26_4,16).toUpper());
        break;
    }case 27:{
        int ana_reserve_out_27_1 = data & 0x03;
        Analog_lineEdit[21].setText(QString::number(ana_reserve_out_27_1,16).toUpper());
        int ana_reserve_out_27_2 = (data & 0x0C)>>2;
        Analog_lineEdit[22].setText(QString::number(ana_reserve_out_27_2,16).toUpper());
        int ana_reserve_out_27_3 = (data & 0x10)>>4;
        Analog_lineEdit[23].setText(QString::number(ana_reserve_out_27_3,16).toUpper());
        int ana_reserve_out_27_4 = (data & 0x60)>>5;
        Analog_lineEdit[24].setText(QString::number(ana_reserve_out_27_4,16).toUpper());
        int ana_reserve_out_27_5 = (data & 0x80)>>7;
        Analog_lineEdit[25].setText(QString::number(ana_reserve_out_27_5,16).toUpper());
        break;
    }case 28:{
        int pixel_qch_bias_ctrl = (data & 0XF0)>>4;
        Pixel_lineEdit[0].setText(QString::number(pixel_qch_bias_ctrl,16).toUpper());
        int pixel_cd_bias_ctrl = data & 0X0F;
        Pixel_lineEdit[1].setText(QString::number(pixel_cd_bias_ctrl,16).toUpper());
        break;
    }case 29:{
        int pixel_cntr_enb = (data & 0x10)>>4;
        Pixel_lineEdit[2].setText(QString::number(pixel_cntr_enb,16).toUpper());
        int pixel_bypass = (data & 0x08)>>3;
        Pixel_lineEdit[3].setText(QString::number(pixel_bypass,16).toUpper());
        int pixel_th1 = (data & 0x04)>>2;
        Pixel_lineEdit[4].setText(QString::number(pixel_th1,16).toUpper());
        int pixel_th0 = (data & 0x02)>>1;
        Pixel_lineEdit[5].setText(QString::number(pixel_th0,16).toUpper());
        int pixel_mode = data & 0x01;
        Pixel_lineEdit[6].setText(QString::number(pixel_mode,16).toUpper());
        break;
    }case 30:{
        int pixel_col_sel_2 = (data & 0xF0)>>4;
        Pixel_lineEdit[7].setText(QString::number(pixel_col_sel_2,16).toUpper());
        int pixel_col_sel_1 = data & 0x0F;
        Pixel_lineEdit[8].setText(QString::number(pixel_col_sel_1,16).toUpper());
        break;
    }case 31:{
        int pixel_reserve_out_31_1 = data & 0x1F;
        Pixel_lineEdit[9].setText(QString::number(pixel_reserve_out_31_1,16).toUpper());
        int pixel_reserve_out_31_2 = (data & 0xE0)>>5;
        Pixel_lineEdit[10].setText(QString::number(pixel_reserve_out_31_2,16).toUpper());
        break;
    }case 32:{
        int pixel_reserve_out_32_1 = data & 0x01;
        Pixel_lineEdit[11].setText(QString::number(pixel_reserve_out_32_1,16).toUpper());
        int pixel_reserve_out_32_2 = (data & 0x02)>>1;
        Pixel_lineEdit[12].setText(QString::number(pixel_reserve_out_32_2,16).toUpper());
        int pixel_reserve_out_32_3 = (data & 0x04)>>2;
        Pixel_lineEdit[13].setText(QString::number(pixel_reserve_out_32_3,16).toUpper());
        int pixel_reserve_out_32_4 = (data & 0x08)>>3;
        Pixel_lineEdit[14].setText(QString::number(pixel_reserve_out_32_4,16).toUpper());
        int pixel_reserve_out_32_5 = (data & 0x70)>>4;
        Pixel_lineEdit[15].setText(QString::number(pixel_reserve_out_32_5,16).toUpper());
        int pixel_reserve_out_32_6 = (data & 0x80)>>7;
        Pixel_lineEdit[16].setText(QString::number(pixel_reserve_out_32_6,16).toUpper());
        break;
    }case 33:{
        int top_reserve_out_33_1 = data & 0x0F;
        Top_lineEdit[0].setText(QString::number(top_reserve_out_33_1,16).toUpper());
        int top_reserve_out_33_2 = (data & 0x10)>>4;
        Top_lineEdit[1].setText(QString::number(top_reserve_out_33_2,16).toUpper());
        int top_reserve_out_33_3 = (data & 0xE0)>>5;
        Top_lineEdit[2].setText(QString::number(top_reserve_out_33_3,16).toUpper());
        break;
    }case 34:{
        int top_reserve_out_34_1 = data & 0x01;
        Top_lineEdit[3].setText(QString::number(top_reserve_out_34_1,16).toUpper());
        int top_reserve_out_34_2 = (data & 0x1E)>>1;
        Top_lineEdit[4].setText(QString::number(top_reserve_out_34_2,16).toUpper());
        int top_reserve_out_34_3 = (data & 0xE0)>>5;
        Top_lineEdit[5].setText(QString::number(top_reserve_out_34_3,16).toUpper());
        break;
    }case 35:{
        int top_reserve_out_35_1 = data & 0x07;
        Top_lineEdit[6].setText(QString::number(top_reserve_out_35_1,16).toUpper());
        int top_reserve_out_35_2 = (data & 0xF8)>>3;
        Top_lineEdit[7].setText(QString::number(top_reserve_out_35_2,16).toUpper());
        break;
    }case 36:{
        int top_reserve_out_36_1 = data & 0x01;
        Top_lineEdit[8].setText(QString::number(top_reserve_out_36_1,16).toUpper());
        int top_reserve_out_36_2 = (data & 0x06)>>1;
        Top_lineEdit[9].setText(QString::number(top_reserve_out_36_2,16).toUpper());
        int top_reserve_out_36_3 = (data & 0x18)>>3;
        Top_lineEdit[10].setText(QString::number(top_reserve_out_36_3,16).toUpper());
        int top_reserve_out_36_4 = (data & 0x60)>>5;
        Top_lineEdit[11].setText(QString::number(top_reserve_out_36_4,16).toUpper());
        int top_reserve_out_36_5 = (data & 0x80)>>7;
        Top_lineEdit[12].setText(QString::number(top_reserve_out_36_5,16).toUpper());
        break;
    }case 37:{
        int r_test_dl_in_en = (data & 0xF8)>>3;
        Delayline_lineEdit[0].setText(QString::number(r_test_dl_in_en,16).toUpper());
        int r_test_dl_width = data & 0x07;
        Delayline_lineEdit[1].setText(QString::number(r_test_dl_width,16).toUpper());
        break;
    }case 38:{
        int r_test_dl_in_0 = data &0xFF;
        Delayline_lineEdit[2].setText(QString::number(r_test_dl_in_0,16).toUpper());
        break;
    }case 39:{
        int r_test_dl_in_1 = data &0xFF;
        Delayline_lineEdit[3].setText(QString::number(r_test_dl_in_1,16).toUpper());
        break;
    }case 40:{
        int r_test_dl_in_2 = data &0xFF;
        Delayline_lineEdit[4].setText(QString::number(r_test_dl_in_2,16).toUpper());
        break;
    }case 41:{
        int r_test_dl_in_3 = data &0xFF;
        Delayline_lineEdit[5].setText(QString::number(r_test_dl_in_3,16).toUpper());
        break;
    }case 42:{
        int r_test_dl_in_4 = data &0xFF;
        Delayline_lineEdit[6].setText(QString::number(r_test_dl_in_4,16).toUpper());
        break;
    }case 43:{
        int r_tdc_start_rise_state = (data & 0x80)>>7;
        MISC_lineEdit[0].setText(QString::number(r_tdc_start_rise_state,16).toUpper());
        int r_tdc_start_rise = (data & 0x70)>>4;
        MISC_lineEdit[1].setText(QString::number(r_tdc_start_rise,16).toUpper());
        int r_tdc_start_fall_state = (data & 0x08)>>3;
        MISC_lineEdit[2].setText(QString::number(r_tdc_start_fall_state,16).toUpper());
        int r_tdc_start_fall = data & 0x07;
        MISC_lineEdit[3].setText(QString::number(r_tdc_start_fall,16).toUpper());
        break;
    }case 44:{
        int r_pds = data & 0xFF;
        MISC_lineEdit[4].setText(QString::number(r_pds,16).toUpper());
        break;
    }case 45:{
        int r_pds = data & 0xFF;
        MISC_lineEdit[5].setText(QString::number(r_pds,16).toUpper());
        break;
    }case 46:{
        int r_pds = data & 0xFF;
        MISC_lineEdit[6].setText(QString::number(r_pds,16).toUpper());
        break;
    }case 47:{
        int r_pds = data & 0xFF;
        MISC_lineEdit[7].setText(QString::number(r_pds,16).toUpper());
        break;
    }case 48:{
        int pll_pwdn = (data & 0x80)>>7;
        Others_lineEdit[0].setText(QString::number(pll_pwdn,16).toUpper());
        int pll_test_en = (data & 0x40)>>6;
        Others_lineEdit[1].setText(QString::number(pll_test_en,16).toUpper());
        int pll_div_rst = (data & 0x20)>>5;
        Others_lineEdit[2].setText(QString::number(pll_div_rst,16).toUpper());
        int r_reduce_cnt = (data & 0x10)>>4;
        Others_lineEdit[3].setText(QString::number(r_reduce_cnt,16).toUpper());
        int Noise_reg = data & 0x0F;
        Others_lineEdit[4].setText(QString::number(Noise_reg,16).toUpper());
        break;
    }case 49:{
        int rhigh_hts = data &0XFF;
        Others_lineEdit[5].setText(QString::number(rhigh_hts,16).toUpper());
    }



    }


}



/************************QtreeWidget 触发相关的槽函数***********************************/

//对于读取寄存器 address registerAddress data 都是十进制
//对于写入寄存器 address resisterAddress 都是十进制数   data是十六进制数


//QTreeWidget 展开时触发的槽函数
//根据文字的内容获取标识，然后读取所有的寄存器的内容 方便下一步修改
void MainWindow::on_treeWidget_itemExpanded(QTreeWidgetItem *item)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }

    int hardWareAddress = 216;     //统一的为0xD8
    qDebug()<<"item on_treeWidget_itemExpanded "<<item->text(0);

    if("TDC" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,0,false);
        emit readDevSignal(hardWareAddress,1,false);
        emit readDevSignal(hardWareAddress,2,false);
        emit readDevSignal(hardWareAddress,3,false);
        emit readDevSignal(hardWareAddress,4,false);
        emit readDevSignal(hardWareAddress,5,false);
    }else if("Integration" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,6,false);
        emit readDevSignal(hardWareAddress,7,false);
        emit readDevSignal(hardWareAddress,8,false);
    }else if("MA" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,9,false);
        emit readDevSignal(hardWareAddress,10,false);
        emit readDevSignal(hardWareAddress,11,false);
        emit readDevSignal(hardWareAddress,12,false);
        emit readDevSignal(hardWareAddress,13,false);
        emit readDevSignal(hardWareAddress,14,false);
        emit readDevSignal(hardWareAddress,15,false);
        emit readDevSignal(hardWareAddress,16,false);
    }else if("Digital" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,17,false);
        emit readDevSignal(hardWareAddress,18,false);
        emit readDevSignal(hardWareAddress,19,false);
        emit readDevSignal(hardWareAddress,20,false);
    }else if("Analog" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,21,false);
        emit readDevSignal(hardWareAddress,22,false);
        emit readDevSignal(hardWareAddress,23,false);
        emit readDevSignal(hardWareAddress,24,false);
        emit readDevSignal(hardWareAddress,25,false);
        emit readDevSignal(hardWareAddress,26,false);
        emit readDevSignal(hardWareAddress,27,false);
    }else if("Pixel" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,28,false);
        emit readDevSignal(hardWareAddress,29,false);
        emit readDevSignal(hardWareAddress,30,false);
        emit readDevSignal(hardWareAddress,31,false);
        emit readDevSignal(hardWareAddress,32,false);
    }else if("Top" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,33,false);
        emit readDevSignal(hardWareAddress,34,false);
        emit readDevSignal(hardWareAddress,35,false);
        emit readDevSignal(hardWareAddress,36,false);
    }else if("Delayline" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,37,false);
        emit readDevSignal(hardWareAddress,38,false);
        emit readDevSignal(hardWareAddress,39,false);
        emit readDevSignal(hardWareAddress,40,false);
        emit readDevSignal(hardWareAddress,41,false);
        emit readDevSignal(hardWareAddress,42,false);
    }else if("MISC" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,43,false);
        emit readDevSignal(hardWareAddress,44,false);
        emit readDevSignal(hardWareAddress,45,false);
        emit readDevSignal(hardWareAddress,46,false);
        emit readDevSignal(hardWareAddress,47,false);
    }else if("Others" == item->text(0))
    {
        emit readDevSignal(hardWareAddress,48,false);
        emit readDevSignal(hardWareAddress,49,false);
    }

}


//点击某一个item时，触发的槽函数，方便后期做注释
void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    qDebug()<<"item on_treeWidget_itemClicked "<<item->text(0);
}


