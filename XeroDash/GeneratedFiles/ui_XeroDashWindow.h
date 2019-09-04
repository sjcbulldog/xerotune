/********************************************************************************
** Form generated from reading UI file 'XeroDashWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XERODASHWINDOW_H
#define UI_XERODASHWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "editabletabwidget.h"

QT_BEGIN_NAMESPACE

class Ui_XeroDashWindowClass
{
public:
    QAction *action_new_tab_;
    QAction *action_save_layout;
    QAction *action_load_layout_;
    QAction *action_preferences_;
    QAction *action_help_about_;
    QAction *action_exit_;
    QAction *action_graph_title_;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter_;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QListWidget *plots_;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QListWidget *nodes_;
    EditableTabWidget *graphs_;
    QWidget *tab;
    QWidget *tab_2;
    QMenuBar *menuBar;
    QMenu *menuView;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuEdit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *XeroDashWindowClass)
    {
        if (XeroDashWindowClass->objectName().isEmpty())
            XeroDashWindowClass->setObjectName(QString::fromUtf8("XeroDashWindowClass"));
        XeroDashWindowClass->resize(1092, 760);
        action_new_tab_ = new QAction(XeroDashWindowClass);
        action_new_tab_->setObjectName(QString::fromUtf8("action_new_tab_"));
        action_save_layout = new QAction(XeroDashWindowClass);
        action_save_layout->setObjectName(QString::fromUtf8("action_save_layout"));
        action_load_layout_ = new QAction(XeroDashWindowClass);
        action_load_layout_->setObjectName(QString::fromUtf8("action_load_layout_"));
        action_preferences_ = new QAction(XeroDashWindowClass);
        action_preferences_->setObjectName(QString::fromUtf8("action_preferences_"));
        action_help_about_ = new QAction(XeroDashWindowClass);
        action_help_about_->setObjectName(QString::fromUtf8("action_help_about_"));
        action_exit_ = new QAction(XeroDashWindowClass);
        action_exit_->setObjectName(QString::fromUtf8("action_exit_"));
        action_graph_title_ = new QAction(XeroDashWindowClass);
        action_graph_title_->setObjectName(QString::fromUtf8("action_graph_title_"));
        centralWidget = new QWidget(XeroDashWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter_ = new QSplitter(centralWidget);
        splitter_->setObjectName(QString::fromUtf8("splitter_"));
        splitter_->setOrientation(Qt::Horizontal);
        verticalLayoutWidget = new QWidget(splitter_);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        plots_ = new QListWidget(verticalLayoutWidget);
        plots_->setObjectName(QString::fromUtf8("plots_"));
        plots_->setAcceptDrops(false);

        verticalLayout->addWidget(plots_);

        splitter_->addWidget(verticalLayoutWidget);
        verticalLayoutWidget_2 = new QWidget(splitter_);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(verticalLayoutWidget_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_2);

        nodes_ = new QListWidget(verticalLayoutWidget_2);
        nodes_->setObjectName(QString::fromUtf8("nodes_"));

        verticalLayout_2->addWidget(nodes_);

        splitter_->addWidget(verticalLayoutWidget_2);
        graphs_ = new EditableTabWidget(splitter_);
        graphs_->setObjectName(QString::fromUtf8("graphs_"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        graphs_->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        graphs_->addTab(tab_2, QString());
        splitter_->addWidget(graphs_);

        horizontalLayout->addWidget(splitter_);

        XeroDashWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(XeroDashWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1092, 18));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        XeroDashWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(XeroDashWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        XeroDashWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(XeroDashWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        XeroDashWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuView->addAction(action_new_tab_);
        menuFile->addAction(action_save_layout);
        menuFile->addAction(action_load_layout_);
        menuFile->addSeparator();
        menuFile->addAction(action_preferences_);
        menuFile->addSeparator();
        menuFile->addAction(action_exit_);
        menuHelp->addAction(action_help_about_);
        menuEdit->addAction(action_graph_title_);

        retranslateUi(XeroDashWindowClass);

        graphs_->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(XeroDashWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *XeroDashWindowClass)
    {
        XeroDashWindowClass->setWindowTitle(QApplication::translate("XeroDashWindowClass", "XeroDashWindow", nullptr));
        action_new_tab_->setText(QApplication::translate("XeroDashWindowClass", "New Tab", nullptr));
        action_save_layout->setText(QApplication::translate("XeroDashWindowClass", "Save Layout ...", nullptr));
        action_load_layout_->setText(QApplication::translate("XeroDashWindowClass", "Load Layout ...", nullptr));
        action_preferences_->setText(QApplication::translate("XeroDashWindowClass", "Preferences ...", nullptr));
        action_help_about_->setText(QApplication::translate("XeroDashWindowClass", "About", nullptr));
        action_exit_->setText(QApplication::translate("XeroDashWindowClass", "Exit", nullptr));
        action_graph_title_->setText(QApplication::translate("XeroDashWindowClass", "Graph Title ...", nullptr));
        label->setText(QApplication::translate("XeroDashWindowClass", "Data Sets", nullptr));
        label_2->setText(QApplication::translate("XeroDashWindowClass", "Values", nullptr));
        graphs_->setTabText(graphs_->indexOf(tab), QApplication::translate("XeroDashWindowClass", "Tab 1", nullptr));
        graphs_->setTabText(graphs_->indexOf(tab_2), QApplication::translate("XeroDashWindowClass", "Tab 2", nullptr));
        menuView->setTitle(QApplication::translate("XeroDashWindowClass", "View", nullptr));
        menuFile->setTitle(QApplication::translate("XeroDashWindowClass", "File", nullptr));
        menuHelp->setTitle(QApplication::translate("XeroDashWindowClass", "Help", nullptr));
        menuEdit->setTitle(QApplication::translate("XeroDashWindowClass", "Edit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class XeroDashWindowClass: public Ui_XeroDashWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XERODASHWINDOW_H
