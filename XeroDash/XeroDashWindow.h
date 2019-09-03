#pragma once

#include "NetworkTableMonitor.h"
#include "PlotManager.h"
#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QSettings>
#include "ui_XeroDashWindow.h"
#include <mutex>
#include <list>
#include <map>

class PlotContainer;

class XeroDashWindow : public QMainWindow
{
	Q_OBJECT

public:
	XeroDashWindow(QWidget *parent = Q_NULLPTR);

protected:
	void closeEvent(QCloseEvent* ev);

private:
	static constexpr int timerTickMS = 250;

private:
	void timerProc();
	void updatePlot(std::shared_ptr<PlotDescriptor> desc);
	void newTab();
	void editPreferences();
	void closeTab(int which);

private:
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windowstate";
	static constexpr const char* TopSplitterSettings = "topsplitter";

	static constexpr const char* PrefDialogUnits = "Units";
	static constexpr const char* NTIPAddress = "IP Address Network Tables";
	static constexpr const char* NTPlotTable = "Name of table with plot data";

	static constexpr const char* ContainerPropName = "plot";

private:
	Ui::XeroDashWindowClass ui;
	NetworkTableMonitor monitor_;
	QTimer* timer_;
	PlotManager* plot_mgr_;
	QSettings settings_;
	std::mutex lock_;
	int count_;
	QString units_;
	QString ipaddr_;
	QString table_name_;

	std::list<std::shared_ptr<PlotDescriptor>> update_list_;
};
