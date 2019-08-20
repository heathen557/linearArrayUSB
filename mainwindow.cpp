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
    MA_widgetItem[2] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w2(a)[3:0]")));
    MA_widgetItem[3] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w3(a)[7:4]")));
    MA_widgetItem[4] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w4(b)[3:0]")));
    MA_widgetItem[5] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w5(b)[7:4]")));
    MA_widgetItem[6] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w6(c)[3:0]")));
    MA_widgetItem[7] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w7(c)[7:4]")));
    MA_widgetItem[8] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w8(d)[3:0]")));
    MA_widgetItem[9] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_w9(d)[7:4]")));
    MA_widgetItem[10] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wa(e)[3:0]")));
    MA_widgetItem[11] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wb(e)[7:4]")));
    MA_widgetItem[12] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wc(f)[3:0]")));
    MA_widgetItem[13] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wd(f)[7:4]")));
    MA_widgetItem[14] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_we(10)[3:0]")));
    MA_widgetItem[15] = new QTreeWidgetItem(MA,QStringList(QString("r_ma_wf(10)[7:4]")));
    MA_read_signalMapper = new QSignalMapper(this);
    MA_write_signalMapper = new QSignalMapper(this);

    for(i=0; i<16; i++)
    {
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
    Digital_widgetItem[0] = new QTreeWidgetItem(Digital,QStringList(QString("r_spi_en(11)[7]")));
    Digital_widgetItem[1] = new QTreeWidgetItem(Digital,QStringList(QString("r_dvp_clk_sel(11)[6:4]")));
    Digital_widgetItem[2] = new QTreeWidgetItem(Digital,QStringList(QString("r_clk_divider(11)[3:0]")));
    Digital_widgetItem[3] = new QTreeWidgetItem(Digital,QStringList(QString("r_sramout_clksel(12)[7]")));
    Digital_widgetItem[4] = new QTreeWidgetItem(Digital,QStringList(QString("r_raw_out_mode(12)[6]")));
    Digital_widgetItem[5] = new QTreeWidgetItem(Digital,QStringList(QString("r_dvp_sram_output_mode(12)[5]")));
    Digital_widgetItem[6] = new QTreeWidgetItem(Digital,QStringList(QString("r_sram_output_cycles(12)[4:0]")));
    Digital_widgetItem[7] = new QTreeWidgetItem(Digital,QStringList(QString("r_row_start(13)[4:0]")));
    Digital_widgetItem[8] = new QTreeWidgetItem(Digital,QStringList(QString("r_high_bits(13)[7]")));
    Digital_widgetItem[9] = new QTreeWidgetItem(Digital,QStringList(QString("r_row_end(14)[5:0]")));
    Digital_read_signalMapper = new QSignalMapper(this);
    Digital_write_signalMapper= new QSignalMapper(this);
    for(i=0 ;i<10; i++)
    {
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
    QTreeWidgetItem *Analog_widgetItem[16];
    Analog_widgetItem[0] = new QTreeWidgetItem(Analog,QStringList(QString("dl_sel_dly(15)[2:0]")));
    Analog_widgetItem[1] = new QTreeWidgetItem(Analog,QStringList(QString("dl_sel_long(15)[3]")));
    Analog_widgetItem[2] = new QTreeWidgetItem(Analog,QStringList(QString("dl_en(15)[4]")));
    Analog_widgetItem[3] = new QTreeWidgetItem(Analog,QStringList(QString("tdc_syncnt_en_global(15)[5]")));
    Analog_widgetItem[4] = new QTreeWidgetItem(Analog,QStringList(QString("tdc_ckdrv_en(15)[6]")));
    Analog_widgetItem[5] = new QTreeWidgetItem(Analog,QStringList(QString("sel_cnt_mode(15)[7]")));
    Analog_widgetItem[6] = new QTreeWidgetItem(Analog,QStringList(QString("pll_coarse_cnt_cksel(16)[4:0]")));
    Analog_widgetItem[7] = new QTreeWidgetItem(Analog,QStringList(QString("pll_lpf_rc(16)[7:6]")));
    Analog_widgetItem[8] = new QTreeWidgetItem(Analog,QStringList(QString("pll_div_ctrl(17)[6:0]")));
    Analog_widgetItem[9] = new QTreeWidgetItem(Analog,QStringList(QString("enb_pclk(18)[7]")));
    Analog_widgetItem[10] = new QTreeWidgetItem(Analog,QStringList(QString("r_tdc_start_re(18)[6]")));
    Analog_widgetItem[11] = new QTreeWidgetItem(Analog,QStringList(QString("mclk_div_rst(18)[5]")));
    Analog_widgetItem[12] = new QTreeWidgetItem(Analog,QStringList(QString("mclk_div_ctrl(18)[4:0]")));
    Analog_widgetItem[13] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out(19)[7:0]")));
    Analog_widgetItem[14] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out(1a)[7:0]")));
    Analog_widgetItem[15] = new QTreeWidgetItem(Analog,QStringList(QString("ana_reserve_out(1b)[7:0]")));
    Analog_read_signalMapper = new QSignalMapper(this);
    Analog_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<16; i++)
    {
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
    QTreeWidgetItem *Pixel_widgetItem[11];
    Pixel_widgetItem[0] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_qch_bias_ctrl(28)[7:4]")));
    Pixel_widgetItem[1] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_cd_bias_ctrl(28)[3:0]")));
    Pixel_widgetItem[2] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_cntr_enb(29)[4]")));
    Pixel_widgetItem[3] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_bypass(29)[3]")));
    Pixel_widgetItem[4] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_th1(29)[2]")));
    Pixel_widgetItem[5] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_th0(29)[1]")));
    Pixel_widgetItem[6] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_mode(29)[0]")));
    Pixel_widgetItem[7] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_col_sel_2(30)[7:4]")));
    Pixel_widgetItem[8] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_col_sel_1(30)[3:0]")));
    Pixel_widgetItem[9] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out(31)[7:0]")));
    Pixel_widgetItem[10] = new QTreeWidgetItem(Pixel,QStringList(QString("pixel_reserve_out(32)[7:0]")));
    Pixel_read_signalMapper = new QSignalMapper(this);
    Pixel_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<11; i++)
    {
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
    QTreeWidgetItem *Top_widgetItem[4];
    Top_widgetItem[0] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out(33)[7:0]")));
    Top_widgetItem[1] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out(34)[7:0]")));
    Top_widgetItem[2] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out(35)[7:0]")));
    Top_widgetItem[3] = new QTreeWidgetItem(Top,QStringList(QString("top_reserve_out(36)[7:0]")));
    Top_read_signalMapper = new QSignalMapper(this);
    Top_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<4; i++)
    {
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
    Others_widgetItem[0] = new QTreeWidgetItem(Others,QStringList(QString("r_tdc_start_rise_state(43)[7]")));
    Others_widgetItem[1] = new QTreeWidgetItem(Others,QStringList(QString("r_tdc_start_rise(43)[6:4]")));
    Others_widgetItem[2] = new QTreeWidgetItem(Others,QStringList(QString("r_tdc_start_fall_state(43)[3]")));
    Others_widgetItem[3] = new QTreeWidgetItem(Others,QStringList(QString("r_tdc_start_fall(43)[2:0]")));
    Others_widgetItem[4] = new QTreeWidgetItem(Others,QStringList(QString("r_pds(44)[7:0]")));
    Others_widgetItem[5] = new QTreeWidgetItem(Others,QStringList(QString("r_pds(45)[7:0]")));
    Others_widgetItem[6] = new QTreeWidgetItem(Others,QStringList(QString("r_pds(46)[7:0]")));
    Others_read_signalMapper = new QSignalMapper(this);
    Others_write_signalMapper = new QSignalMapper(this);
    for(i=0; i<7; i++)
    {
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
}
//写入 MA 槽函数
void MainWindow::MA_write_slot(int MA_number)
{
    if(!isLinkSuccess)
    {
        QMessageBox::information(NULL,QStringLiteral("告警"),QStringLiteral("设备未连接"));
        return;
    }
    qDebug()<<"write MA_number = "<<MA_number<<endl;
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
    }

}


//点击某一个item时，触发的槽函数，方便后期做注释
void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    qDebug()<<"item on_treeWidget_itemClicked "<<item->text(0);
}


