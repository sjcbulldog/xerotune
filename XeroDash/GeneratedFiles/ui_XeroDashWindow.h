/********************************************************************************
** Form generated from reading UI file 'XeroDashWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XERODASHWINDOW_H
#define UI_XERODASHWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_XeroDashWindowClass
{
public:
    QAction *action_new_tab_;
    QAction *action_save_layout;
    QAction *action_load_layout_;
    QAction *action_preferences_;
    QAction *action_help_about_;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter_;
    QListWidget *plots_;
    QListWidget *nodes_;
    QTabWidget *graphs_;
    QWidget *tab;
    QWidget *tab_2;
    QMenuBar *menuBar;
    QMenu *menuView;
    QMenu *menuFile;
    QMenu *action_exit_;
    QMenu *menuHelp;
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
        centralWidget = new QWidget(XeroDashWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter_ = new QSplitter(centralWidget);
        splitter_->setObjectName(QString::fromUtf8("splitter_"));
        splitter_->setOrientation(Qt::Horizontal);
        plots_ = new QListWidget(splitter_);
        plots_->setObjectName(QString::fromUtf8("plots_"));
        splitter_->addWidget(plots_);
        nodes_ = new QListWidget(splitter_);
        nodes_->setObjectName(QString::fromUtf8("nodes_"));
        splitter_->addWidget(nodes_);
        graphs_ = new QTabWidget(splitter_);
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
        action_exit_ = new QMenu(menuFile);
        action_exit_->setObjectName(QString::fromUtf8("action_exit_"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        XeroDashWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(XeroDashWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        XeroDashWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(XeroDashWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        XeroDashWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuView->addAction(action_new_tab_);
        menuFile->addAction(action_save_layout);
        menuFile->addAction(action_load_layout_);
        menuFile->addSeparator();
        menuFile->addAction(action_preferences_);
        menuFile->addSeparator();
        menuFile->addAction(action_exit_->menuAction());
        action_exit_->addSeparator();
        menuHelp->addAction(action_help_about_);

        retranslateUi(XeroDashWindowClass);

        graphs_->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(XeroDashWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *XeroDashWindowClass)
    {
        XeroDashWindowClass->setWindowTitle(QCoreApplication::translate("XeroDashWindowClass", "XeroDashWindow", nullptr));
        action_new_tab_->setText(QCoreApplication::translate("XeroDashWindowClass", "New Tab", nullptr));
        action_save_layout->setText(QCoreApplication::translate("XeroDashWindowClass", "Save Layout ...", nullptr));
        action_load_layout_->setText(QCoreApplication::translate("XeroDashWindowClass", "Load Layout ...", nullptr));
        action_preferences_->setText(QCoreApplication::translate("XeroDashWindowClass", "Preferences ...", nullptr));
        action_help_about_->setText(QCoreApplication::translate("XeroDashWindowClass", "About", nullptr));
        graphs_->setTabText(graphs_->indexOf(tab), QCoreApplication::translate("XeroDashWindowClass", "Tab 1", nullptr));
        graphs_->setTabText(graphs_->indexOf(tab_2), QCoreApplication::translate("XeroDashWindowClass", "Tab 2", nullptr));
        menuView->setTitle(QCoreApplication::translate("XeroDashWindowClass", "View", nullptr));
        menuFile->setTitle(QCoreApplication::translate("XeroDashWindowClass", "File", nullptr));
        action_exit_->setTitle(QCoreApplication::translate("XeroDashWindowClass", "Exit", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("XeroDashWindowClass", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class XeroDashWindowClass: public Ui_XeroDashWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XERODASHWINDOW_H
