#include "widget.h"
#include <QDebug>
#include <regex>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    // 初始化Ui
    ui = new Ui::Widget();
    ui->setupUi(this);
    // 设置TableView Header
    model->setHorizontalHeaderItem(0, new QStandardItem("跃点数"));
    model->setHorizontalHeaderItem(1, new QStandardItem("IP地址"));
    model->setHorizontalHeaderItem(2, new QStandardItem("往返时延(ms)"));
    model->setHorizontalHeaderItem(3, new QStandardItem("地址"));
    model->setHorizontalHeaderItem(4, new QStandardItem("AS"));
    model->setHorizontalHeaderItem(5, new QStandardItem("域名"));
    // 设置表头的显示模式，例如自动调整列宽
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置其他属性，例如表头文字的对齐方式
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableView->setModel(model);
    // 连接信号与槽
    connect(ui->pushButton, &QPushButton::clicked, this, &Widget::on_pushButtonClicked);
}

Widget::~Widget()
{
    delete ui;
    delete model;
    if (myThread01) myThread01->terminate();
    delete myThread01;
}

void Widget::on_pushButtonClicked()
{
    // 对输入的字符串判空
    std::string str = ui->lineEdit->text().toStdString();
//    if (str.length() == 0) return;

    // 先删除表中的数据
    model->removeRows(0, model->rowCount());

    // 再删除旧的线程
    if (myThread01) myThread01->terminate();
    delete myThread01;

    // 创建新的线程
    myThread01 = new MyThread(str.c_str(), 1);

    // 连接信号与槽
    connect(myThread01, &MyThread::signal_myThread_ok, this, &Widget::on_myThreadSignal_ok);

    // 运行线程
    myThread01->start();
}

void Widget::on_myThreadSignal_ok(QList<QString> list)
{
    // 创建新的QStandardItem对象并设置数据
    QStandardItem *item1 = new QStandardItem(list.at(0));
    QStandardItem *item2 = new QStandardItem(list.at(1));
    QStandardItem *item3 = new QStandardItem(list.at(2));
    // 将新的QStandardItem对象添加到模型的新行中
    QList<QStandardItem*> rowItems;
    rowItems << item1 << item2 << item3;
    // 新增一行
    model->appendRow(rowItems);

    qDebug() << list.at(0) << " " << list.at(1) << " " << list.at(2);
}
