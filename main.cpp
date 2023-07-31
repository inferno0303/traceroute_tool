#include <QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    // 适配高DPI缩放，Qt5.6 ~ 5.13版本适用
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
