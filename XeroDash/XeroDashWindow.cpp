#include "XeroDashWindow.h"
#include "PlotContainer.h"
#include "PropertyEditor.h"

XeroDashWindow::XeroDashWindow(QWidget *parent) : QMainWindow(parent)
{
	count_ = 0;

	ui.setupUi(this);
	plot_mgr_ = new PlotManager(monitor_, *ui.plots_, *ui.nodes_);
	ui.graphs_->clear();

	(void)connect(ui.graphs_, &QTabWidget::tabCloseRequested, this, &XeroDashWindow::closeTab);

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

	if (settings_.contains(PrefDialogUnits))
		units_ = settings_.value(PrefDialogUnits).toString();
	else
		units_ = "in";

	if (settings_.contains(NTIPAddress))
		ipaddr_ = settings_.value(NTIPAddress).toString();
	else
		ipaddr_ = "127.0.0.1";

	if (settings_.contains(NTPlotTable))
		table_name_ = settings_.value(NTPlotTable).toString();
	else
		table_name_ = "XeroPlot";

	monitor_.setIPAddress(ipaddr_.toStdString());
	monitor_.start(table_name_.toStdString());

	newTab();

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &XeroDashWindow::timerProc);

	timer_->start(timerTickMS);

	//
	// Menuss
	//
	(void)connect(ui.action_new_tab_, &QAction::triggered, this, &XeroDashWindow::newTab);
	(void)connect(ui.action_preferences_, &QAction::triggered, this, &XeroDashWindow::editPreferences);

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
	PlotContainer* cnt = new PlotContainer(*plot_mgr_);
	cnt->setUnits(units_);
	int index = ui.graphs_->addTab(cnt, title);
	QWidget* widget = ui.graphs_->widget(index);
	widget->setProperty(ContainerPropName, QVariant(0, cnt));
}

void XeroDashWindow::editPreferences()
{
	PropertyEditor dialog;
	std::shared_ptr<EditableProperty> prop;

	prop = std::make_shared<EditableProperty>(PrefDialogUnits, EditableProperty::PTStringList,
		QVariant(units_), "The units of measurement");
	prop->addChoice("m");
	prop->addChoice("meters");
	prop->addChoice("cm");
	prop->addChoice("feet");
	prop->addChoice("ft");
	prop->addChoice("inches");
	prop->addChoice("in");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(NTIPAddress, EditableProperty::PTString,
		QVariant(ipaddr_), "The IP address of the Network Table server");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(NTPlotTable, EditableProperty::PTString,
		QVariant(table_name_), "The path (with trailing slash) to the network table where plots are found");
	dialog.getModel().addProperty(prop);

	if (dialog.exec() == QDialog::Rejected)
		return;

	units_ = dialog.getModel().getProperty(PrefDialogUnits)->getValue().toString();
	settings_.setValue(PrefDialogUnits, units_);

	ipaddr_ = dialog.getModel().getProperty(NTIPAddress)->getValue().toString();
	settings_.setValue(NTIPAddress, ipaddr_);

	table_name_ = dialog.getModel().getProperty(NTPlotTable)->getValue().toString();
	if (!table_name_.endsWith("/"))
		table_name_ += "/";
	if (!table_name_.startsWith("/"))
		table_name_ = "/" + table_name_;
	settings_.setValue(NTPlotTable, table_name_);

	if (monitor_.stop(30 * 1000))
	{
		monitor_.setIPAddress(ipaddr_.toStdString());
		monitor_.start(table_name_.toStdString());
	}

	for (size_t i = 0; i < ui.graphs_->count(); i++)
	{
		QWidget* widget = ui.graphs_->widget(i);
		QVariant v = widget->property(ContainerPropName);
		void* ptr = static_cast<void*>(v.value<void*>());
		PlotContainer* cnt = static_cast<PlotContainer*>(ptr);
		cnt->setUnits(units_);
	}
}

void XeroDashWindow::closeTab(int which)
{
	QWidget* widget = ui.graphs_->widget(which);
	QVariant v = widget->property(ContainerPropName);
	void* ptr = static_cast<void*>(v.value<void*>());
	PlotContainer* cnt = static_cast<PlotContainer*>(ptr);
	ui.graphs_->removeTab(which);

	delete cnt;
}
