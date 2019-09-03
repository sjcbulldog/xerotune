#include "XeroDashWindow.h"
#include "PlotContainer.h"

XeroDashWindow::XeroDashWindow(QWidget *parent) : QMainWindow(parent)
{
	monitor_.setIPAddress("127.0.0.1");
	monitor_.start("/XeroPlot/");
	count_ = 1;

	ui.setupUi(this);
	plot_mgr_ = new PlotManager(monitor_, *ui.plots_, *ui.nodes_);

	ui.graphs_->clear();
	ui.graphs_->addTab(new PlotContainer(*plot_mgr_), "Plots");

	if (settings_.contains(GeometrySettings))
		restoreGeometry(settings_.value(GeometrySettings).toByteArray());

	if (settings_.contains(WindowStateSettings))
		restoreState(settings_.value(WindowStateSettings).toByteArray());

	if (settings_.contains(TopSplitterSettings))
	{
		QList<QVariant> stored = settings_.value(TopSplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		ui.splitter_->setSizes(sizes);
	}

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &XeroDashWindow::timerProc);

	timer_->start(timerTickMS);

	//
	// Menuss
	//
	(void)connect(ui.action_new_tab_, &QAction::triggered, this, &XeroDashWindow::newTab);


}

void XeroDashWindow::closeEvent(QCloseEvent* event)
{
	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());

	QList<QVariant> stored;
	for (int size : ui.splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(TopSplitterSettings, stored);
}

void XeroDashWindow::updatePlot(std::shared_ptr<PlotDescriptor> desc)
{
	std::lock_guard<std::mutex> guard(lock_);
	update_list_.push_back(desc);
}

void XeroDashWindow::timerProc()
{
	plot_mgr_->tick();
}

void XeroDashWindow::newTab()
{
	count_++;

	QString title = "Plots (" + QString::number(count_) + ")" ;
	ui.graphs_->addTab(new PlotContainer(*plot_mgr_), title);
}
