/********************************************************************************
** Form generated from reading UI file 'ui_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_WidgetTitle;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QTableView *tableView;
    QLabel *label;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(600, 450);
        Widget->setMinimumSize(QSize(600, 450));
        verticalLayout = new QVBoxLayout(Widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_WidgetTitle = new QLabel(Widget);
        label_WidgetTitle->setObjectName(QString::fromUtf8("label_WidgetTitle"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        label_WidgetTitle->setFont(font);
        label_WidgetTitle->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_WidgetTitle);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        pushButton = new QPushButton(Widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);

        tableView = new QTableView(Widget);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setGridStyle(Qt::NoPen);

        verticalLayout->addWidget(tableView);

        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "\350\267\257\347\224\261\350\277\275\350\270\252\345\267\245\345\205\267 - Traceroute Tool", nullptr));
        label_WidgetTitle->setText(QApplication::translate("Widget", "\350\267\257\347\224\261\350\277\275\350\270\252\345\267\245\345\205\267", nullptr));
        lineEdit->setPlaceholderText(QApplication::translate("Widget", "\350\276\223\345\205\245IP\345\234\260\345\235\200", nullptr));
        pushButton->setText(QApplication::translate("Widget", "\347\241\256\345\256\232", nullptr));
        label->setText(QApplication::translate("Widget", "\350\267\257\347\224\261\350\277\275\350\270\252\345\267\245\345\205\267 TraceRoute Tool", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
