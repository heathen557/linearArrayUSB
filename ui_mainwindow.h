/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action;
    QAction *action_2;
    QAction *action_3;
    QAction *action_4;
    QAction *action_5;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    GLWidget *widget;
    QToolBox *toolBox;
    QWidget *page;
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_5;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_8;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *VID_lineEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *PID_lineEdit;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer;
    QPushButton *linkUSB_pushButton;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_9;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *sysAddress_lineEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_4;
    QLineEdit *sysData_lineEdit;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *readSys_pushButton;
    QPushButton *writeSys_pushButton;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_11;
    QVBoxLayout *verticalLayout_2;
    QPushButton *loadSetting_pushButton;
    QPushButton *saveSetting_pushButton;
    QPushButton *changeTofPeak_pushButton;
    QWidget *page_2;
    QGridLayout *gridLayout_4;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *getALL_pushButton;
    QPushButton *setAll_pushButton;
    QWidget *page_3;
    QGridLayout *gridLayout_7;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *gbox1;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_5;
    QComboBox *serialPortInfoListBox;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_6;
    QComboBox *baudRateBox;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_7;
    QComboBox *dataBitsBox;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_8;
    QComboBox *parityBox;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_9;
    QComboBox *stopBitsBox;
    QPushButton *openSerial_pushButton;
    QPushButton *serialPlay_pushButton;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_5;
    QFrame *frame_6;
    QGridLayout *gridLayout_15;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_11;
    QTableWidget *tableWidget_2;
    QTextEdit *textEdit;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QMenu *menu_5;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1297, 715);
        QIcon icon;
        icon.addFile(QStringLiteral("icon_1.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        action = new QAction(MainWindow);
        action->setObjectName(QStringLiteral("action"));
        action_2 = new QAction(MainWindow);
        action_2->setObjectName(QStringLiteral("action_2"));
        action_3 = new QAction(MainWindow);
        action_3->setObjectName(QStringLiteral("action_3"));
        action_4 = new QAction(MainWindow);
        action_4->setObjectName(QStringLiteral("action_4"));
        action_5 = new QAction(MainWindow);
        action_5->setObjectName(QStringLiteral("action_5"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setStyleSheet(QString::fromUtf8("*{font:10pt \"\351\273\221\344\275\223\";}\n"
"#frame{border:1px solid gray;}\n"
""));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        widget = new GLWidget(frame);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setStyleSheet(QStringLiteral("background-color:black"));

        gridLayout_2->addWidget(widget, 0, 1, 1, 1);

        toolBox = new QToolBox(frame);
        toolBox->setObjectName(QStringLiteral("toolBox"));
        toolBox->setMaximumSize(QSize(250, 16777215));
        toolBox->setStyleSheet(QStringLiteral(""));
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        page->setGeometry(QRect(0, 0, 250, 358));
        gridLayout_3 = new QGridLayout(page);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(-1, 2, -1, 0);
        pushButton_5 = new QPushButton(page);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setMaximumSize(QSize(250, 23));
        pushButton_5->setStyleSheet(QString::fromUtf8("font:10pt \"\351\273\221\344\275\223\";"));

        gridLayout_3->addWidget(pushButton_5, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(168, 27, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 5, 0, 1, 1);

        groupBox_4 = new QGroupBox(page);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setMaximumSize(QSize(230, 108));
        groupBox_4->setStyleSheet(QString::fromUtf8("font:10pt \"\351\273\221\344\275\223\";"));
        gridLayout_8 = new QGridLayout(groupBox_4);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_8->setContentsMargins(-1, 2, -1, 2);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(groupBox_4);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        VID_lineEdit = new QLineEdit(groupBox_4);
        VID_lineEdit->setObjectName(QStringLiteral("VID_lineEdit"));

        horizontalLayout_2->addWidget(VID_lineEdit);


        gridLayout_8->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(groupBox_4);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        PID_lineEdit = new QLineEdit(groupBox_4);
        PID_lineEdit->setObjectName(QStringLiteral("PID_lineEdit"));

        horizontalLayout_3->addWidget(PID_lineEdit);


        gridLayout_8->addLayout(horizontalLayout_3, 1, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        linkUSB_pushButton = new QPushButton(groupBox_4);
        linkUSB_pushButton->setObjectName(QStringLiteral("linkUSB_pushButton"));
        linkUSB_pushButton->setStyleSheet(QString::fromUtf8("font:10pt \"\351\273\221\344\275\223\";"));

        horizontalLayout_7->addWidget(linkUSB_pushButton);


        gridLayout_8->addLayout(horizontalLayout_7, 2, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_4, 0, 0, 1, 1);

        groupBox_6 = new QGroupBox(page);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setMaximumSize(QSize(230, 108));
        groupBox_6->setStyleSheet(QString::fromUtf8("font:10pt \"\351\273\221\344\275\223\";\n"
""));
        gridLayout_9 = new QGridLayout(groupBox_6);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        gridLayout_9->setContentsMargins(-1, 2, -1, 4);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_3 = new QLabel(groupBox_6);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_4->addWidget(label_3);

        sysAddress_lineEdit = new QLineEdit(groupBox_6);
        sysAddress_lineEdit->setObjectName(QStringLiteral("sysAddress_lineEdit"));

        horizontalLayout_4->addWidget(sysAddress_lineEdit);


        gridLayout_9->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_4 = new QLabel(groupBox_6);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_5->addWidget(label_4);

        sysData_lineEdit = new QLineEdit(groupBox_6);
        sysData_lineEdit->setObjectName(QStringLiteral("sysData_lineEdit"));

        horizontalLayout_5->addWidget(sysData_lineEdit);


        gridLayout_9->addLayout(horizontalLayout_5, 1, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        readSys_pushButton = new QPushButton(groupBox_6);
        readSys_pushButton->setObjectName(QStringLiteral("readSys_pushButton"));

        horizontalLayout_6->addWidget(readSys_pushButton);

        writeSys_pushButton = new QPushButton(groupBox_6);
        writeSys_pushButton->setObjectName(QStringLiteral("writeSys_pushButton"));

        horizontalLayout_6->addWidget(writeSys_pushButton);


        gridLayout_9->addLayout(horizontalLayout_6, 2, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_6, 1, 0, 1, 1);

        groupBox_8 = new QGroupBox(page);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        groupBox_8->setMaximumSize(QSize(16777215, 80));
        gridLayout_11 = new QGridLayout(groupBox_8);
        gridLayout_11->setSpacing(6);
        gridLayout_11->setContentsMargins(11, 11, 11, 11);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        gridLayout_11->setContentsMargins(-1, 2, -1, 4);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        loadSetting_pushButton = new QPushButton(groupBox_8);
        loadSetting_pushButton->setObjectName(QStringLiteral("loadSetting_pushButton"));

        verticalLayout_2->addWidget(loadSetting_pushButton);

        saveSetting_pushButton = new QPushButton(groupBox_8);
        saveSetting_pushButton->setObjectName(QStringLiteral("saveSetting_pushButton"));

        verticalLayout_2->addWidget(saveSetting_pushButton);


        gridLayout_11->addLayout(verticalLayout_2, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_8, 2, 0, 1, 1);

        changeTofPeak_pushButton = new QPushButton(page);
        changeTofPeak_pushButton->setObjectName(QStringLiteral("changeTofPeak_pushButton"));

        gridLayout_3->addWidget(changeTofPeak_pushButton, 4, 0, 1, 1);

        toolBox->addItem(page, QString::fromUtf8("            \345\270\270\347\224\250\351\205\215\347\275\256"));
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        page_2->setGeometry(QRect(0, 0, 250, 358));
        gridLayout_4 = new QGridLayout(page_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        treeWidget = new QTreeWidget(page_2);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setStyleSheet(QStringLiteral("font: 9pt \"Times New Roman\";"));

        gridLayout_4->addWidget(treeWidget, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        getALL_pushButton = new QPushButton(page_2);
        getALL_pushButton->setObjectName(QStringLiteral("getALL_pushButton"));

        horizontalLayout->addWidget(getALL_pushButton);

        setAll_pushButton = new QPushButton(page_2);
        setAll_pushButton->setObjectName(QStringLiteral("setAll_pushButton"));

        horizontalLayout->addWidget(setAll_pushButton);


        gridLayout_4->addLayout(horizontalLayout, 1, 0, 1, 1);

        toolBox->addItem(page_2, QString::fromUtf8("         \350\256\276\345\244\207\345\257\204\345\255\230\345\231\250\351\205\215\347\275\256"));
        page_3 = new QWidget();
        page_3->setObjectName(QStringLiteral("page_3"));
        page_3->setGeometry(QRect(0, 0, 250, 358));
        gridLayout_7 = new QGridLayout(page_3);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        verticalSpacer_2 = new QSpacerItem(20, 158, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer_2, 2, 0, 1, 1);

        gbox1 = new QGroupBox(page_3);
        gbox1->setObjectName(QStringLiteral("gbox1"));
        gridLayout_6 = new QGridLayout(gbox1);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_5 = new QLabel(gbox1);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_8->addWidget(label_5);

        serialPortInfoListBox = new QComboBox(gbox1);
        serialPortInfoListBox->setObjectName(QStringLiteral("serialPortInfoListBox"));

        horizontalLayout_8->addWidget(serialPortInfoListBox);


        gridLayout_6->addLayout(horizontalLayout_8, 0, 0, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_6 = new QLabel(gbox1);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_9->addWidget(label_6);

        baudRateBox = new QComboBox(gbox1);
        baudRateBox->setObjectName(QStringLiteral("baudRateBox"));

        horizontalLayout_9->addWidget(baudRateBox);


        gridLayout_6->addLayout(horizontalLayout_9, 1, 0, 1, 1);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_7 = new QLabel(gbox1);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_10->addWidget(label_7);

        dataBitsBox = new QComboBox(gbox1);
        dataBitsBox->setObjectName(QStringLiteral("dataBitsBox"));

        horizontalLayout_10->addWidget(dataBitsBox);


        gridLayout_6->addLayout(horizontalLayout_10, 2, 0, 1, 1);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_8 = new QLabel(gbox1);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_11->addWidget(label_8);

        parityBox = new QComboBox(gbox1);
        parityBox->setObjectName(QStringLiteral("parityBox"));

        horizontalLayout_11->addWidget(parityBox);


        gridLayout_6->addLayout(horizontalLayout_11, 3, 0, 1, 1);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        label_9 = new QLabel(gbox1);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_12->addWidget(label_9);

        stopBitsBox = new QComboBox(gbox1);
        stopBitsBox->setObjectName(QStringLiteral("stopBitsBox"));

        horizontalLayout_12->addWidget(stopBitsBox);


        gridLayout_6->addLayout(horizontalLayout_12, 4, 0, 1, 1);

        openSerial_pushButton = new QPushButton(gbox1);
        openSerial_pushButton->setObjectName(QStringLiteral("openSerial_pushButton"));
        openSerial_pushButton->setCursor(QCursor(Qt::PointingHandCursor));

        gridLayout_6->addWidget(openSerial_pushButton, 5, 0, 1, 1);


        gridLayout_7->addWidget(gbox1, 0, 0, 1, 1);

        serialPlay_pushButton = new QPushButton(page_3);
        serialPlay_pushButton->setObjectName(QStringLiteral("serialPlay_pushButton"));

        gridLayout_7->addWidget(serialPlay_pushButton, 1, 0, 1, 1);

        toolBox->addItem(page_3, QString::fromUtf8("            \344\270\262\345\217\243\350\277\236\346\216\245"));

        gridLayout_2->addWidget(toolBox, 0, 0, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 150));
        groupBox_2->setMaximumSize(QSize(16777215, 175));
        groupBox_2->setStyleSheet(QString::fromUtf8("\n"
"*{font:10pt \"\351\273\221\344\275\223\";}\n"
"#groupBox_2{\n"
"border:1px solid gray;}"));
        gridLayout_5 = new QGridLayout(groupBox_2);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        frame_6 = new QFrame(groupBox_2);
        frame_6->setObjectName(QStringLiteral("frame_6"));
        frame_6->setMaximumSize(QSize(250, 16777215));
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        gridLayout_15 = new QGridLayout(frame_6);
        gridLayout_15->setSpacing(6);
        gridLayout_15->setContentsMargins(11, 11, 11, 11);
        gridLayout_15->setObjectName(QStringLiteral("gridLayout_15"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_11 = new QLabel(frame_6);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setStyleSheet(QStringLiteral(""));

        verticalLayout_5->addWidget(label_11);

        tableWidget_2 = new QTableWidget(frame_6);
        if (tableWidget_2->columnCount() < 2)
            tableWidget_2->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_2->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_2->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tableWidget_2->rowCount() < 2)
            tableWidget_2->setRowCount(2);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget_2->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget_2->setVerticalHeaderItem(1, __qtablewidgetitem3);
        tableWidget_2->setObjectName(QStringLiteral("tableWidget_2"));
        tableWidget_2->setStyleSheet(QLatin1String("\n"
"border:0px"));

        verticalLayout_5->addWidget(tableWidget_2);


        gridLayout_15->addLayout(verticalLayout_5, 0, 0, 1, 1);


        gridLayout_5->addWidget(frame_6, 0, 0, 1, 1);

        textEdit = new QTextEdit(groupBox_2);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setStyleSheet(QStringLiteral("font: 9pt \"Times New Roman\";"));

        gridLayout_5->addWidget(textEdit, 0, 1, 1, 1);


        gridLayout->addWidget(groupBox_2, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1297, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        menu_4 = new QMenu(menuBar);
        menu_4->setObjectName(QStringLiteral("menu_4"));
        menu_5 = new QMenu(menuBar);
        menu_5->setObjectName(QStringLiteral("menu_5"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menuBar->addAction(menu_5->menuAction());
        menuBar->addAction(menu_4->menuAction());
        menu->addAction(action);
        menu->addAction(action_5);
        menu_3->addAction(action_2);
        menu_5->addAction(action_4);

        retranslateUi(MainWindow);

        toolBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\350\212\257\350\247\206\347\225\214\347\272\277\351\230\265\350\256\276\345\244\207\346\265\213\350\257\225\347\250\213\345\272\217", Q_NULLPTR));
        action->setText(QApplication::translate("MainWindow", "\346\226\207\344\273\266\344\277\235\345\255\230", Q_NULLPTR));
        action_2->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\350\256\276\347\275\256", Q_NULLPTR));
        action_3->setText(QApplication::translate("MainWindow", "\347\273\237\350\256\241\350\256\276\347\275\256", Q_NULLPTR));
        action_4->setText(QApplication::translate("MainWindow", "\347\273\237\350\256\241\344\277\241\346\201\257", Q_NULLPTR));
        action_5->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\234\254\345\234\260\346\226\207\344\273\266", Q_NULLPTR));
        pushButton_5->setText(QApplication::translate("MainWindow", "\346\222\255\346\224\276", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "\350\256\276\345\244\207\350\277\236\346\216\245\357\274\232", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "VID:", Q_NULLPTR));
        VID_lineEdit->setText(QApplication::translate("MainWindow", "8888", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "PID ", Q_NULLPTR));
        PID_lineEdit->setText(QApplication::translate("MainWindow", "0160", Q_NULLPTR));
        linkUSB_pushButton->setText(QApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "\347\263\273\347\273\237\345\257\204\345\255\230\345\231\250\357\274\232", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        sysAddress_lineEdit->setText(QApplication::translate("MainWindow", "13", Q_NULLPTR));
        label_4->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\357\274\232", Q_NULLPTR));
        readSys_pushButton->setText(QApplication::translate("MainWindow", "\350\257\273\345\217\226", Q_NULLPTR));
        writeSys_pushButton->setText(QApplication::translate("MainWindow", "\345\206\231\345\205\245", Q_NULLPTR));
        groupBox_8->setTitle(QApplication::translate("MainWindow", "\351\205\215\347\275\256\351\233\206\357\274\232", Q_NULLPTR));
        loadSetting_pushButton->setText(QApplication::translate("MainWindow", "\345\212\240\350\275\275\351\205\215\347\275\256\351\233\206", Q_NULLPTR));
        saveSetting_pushButton->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\351\205\215\347\275\256\351\233\206", Q_NULLPTR));
        changeTofPeak_pushButton->setText(QApplication::translate("MainWindow", "\345\210\207\346\215\242TOF/PEAK", Q_NULLPTR));
        toolBox->setItemText(toolBox->indexOf(page), QApplication::translate("MainWindow", "            \345\270\270\347\224\250\351\205\215\347\275\256", Q_NULLPTR));
        getALL_pushButton->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\350\257\273\345\217\226", Q_NULLPTR));
        setAll_pushButton->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\206\231\345\205\245", Q_NULLPTR));
        toolBox->setItemText(toolBox->indexOf(page_2), QApplication::translate("MainWindow", "         \350\256\276\345\244\207\345\257\204\345\255\230\345\231\250\351\205\215\347\275\256", Q_NULLPTR));
        gbox1->setTitle(QApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\202\346\225\260\350\256\276\347\275\256", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267\357\274\232", Q_NULLPTR));
        label_6->setText(QApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207\357\274\232", Q_NULLPTR));
        label_7->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215\357\274\232", Q_NULLPTR));
        label_8->setText(QApplication::translate("MainWindow", "\346\240\241\351\252\214\344\275\215\357\274\232", Q_NULLPTR));
        label_9->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215\357\274\232", Q_NULLPTR));
        openSerial_pushButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", Q_NULLPTR));
        serialPlay_pushButton->setText(QApplication::translate("MainWindow", "\346\222\255\346\224\276", Q_NULLPTR));
        toolBox->setItemText(toolBox->indexOf(page_3), QApplication::translate("MainWindow", "            \344\270\262\345\217\243\350\277\236\346\216\245", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "\347\273\237\350\256\241\344\277\241\346\201\257\344\270\216\346\227\245\345\277\227", Q_NULLPTR));
        label_11->setText(QApplication::translate("MainWindow", "        TOF/PEAK\344\277\241\346\201\257\347\273\237\350\256\241", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_2->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "minValue", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_2->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "maxValue", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget_2->verticalHeaderItem(0);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "tof", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget_2->verticalHeaderItem(1);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "peak", Q_NULLPTR));
        menu->setTitle(QApplication::translate("MainWindow", "\346\226\207\344\273\266", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("MainWindow", "\347\233\270\346\234\272\345\261\236\346\200\247", Q_NULLPTR));
        menu_3->setTitle(QApplication::translate("MainWindow", "\350\256\276\347\275\256", Q_NULLPTR));
        menu_4->setTitle(QApplication::translate("MainWindow", "\345\270\256\345\212\251", Q_NULLPTR));
        menu_5->setTitle(QApplication::translate("MainWindow", "\350\260\203\350\257\225", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
