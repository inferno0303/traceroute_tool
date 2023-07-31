#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QStandardItemModel>

#include "ui/ui_widget.h"
#include "threads/MyThread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QStandardItemModel *model = new QStandardItemModel();
    MyThread *myThread01 = nullptr;
    void on_pushButtonClicked();
    void on_myThreadSignal_ok(QList<QString>);
};
#endif // WIDGET_H
