#include "XeroDashWindow.h"
#include "PlotContainer.h"
#include "PropertyEditor.h"
#include "AboutDialog.h"
#include "KeyHints.h"
#include <QtWidgets/QLineEdit>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

XeroDashWindow::XeroDashWindow(QWidget *parent) : QMainWindow(parent)
{
	count_ = 1;
	editor_ = nullptr;

	ui.setupUi(this);
	ui.graphs_->clear();

	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/icon.png";
	QPixmap image(imagepath);
	QIcon icon(image);
	setWindowIcon(icon);

	(void)connect(ui.graphs_, &QTabWidget::tabCloseRequested, this, &XeroDashWindow::closeTab);
	(void)connect(ui.graphs_->tabBar(), &QTabBar::tabBarDoubleClicked, this, &XeroDashWindow::editTab);

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
		table_name_ = "/XeroPlot/";

	monitor_ = new NetworkTableMonitor(collection_, ipaddr_.toStdString(), table_name_.toStdString());
	plot_mgr_ = new PlotManager(collection_, *ui.plots_, *ui.nodes_);
	plot_mgr_->setNetworkMonitor(monitor_);

	newTab();

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &XeroDashWindow::timerProc);

	timer_->start(timerTickMS);

	//
	// Menuss
	//
	(void)connect(ui.action_new_tab_, &QAction::triggered, this, &XeroDashWindow::newTab);
	(void)connect(ui.action_preferences_, &QAction::triggered, this, &XeroDashWindow::editPreferences);
	(void)connect(ui.action_exit_, &QAction::triggered, this, &XeroDashWindow::fileExit);
	(void)connect(ui.action_help_about_, &QAction::triggered, this, &XeroDashWindow::helpAbout);
	(void)connect(ui.action_load_layout_, &QAction::triggered, this, &XeroDashWindow::fileLoadLayout);
	(void)connect(ui.action_save_layout, &QAction::triggered, this, &XeroDashWindow::fileSaveLayout);
	(void)connect(ui.action_graph_title_, &QAction::triggered, this, &XeroDashWindow::editGraphTitle);
	(void)connect(ui.action_help_keyboard_, &QAction::triggered, this, &XeroDashWindow::helpKeyboard);

	status_ = new QLabel("Ready");
	statusBar()->addWidget(status_);

	plot_mgr_->setStatusLabel(status_);
}

void XeroDashWindow::closeEvent(QCloseEvent* event)
{
	timer_->stop();

	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());

	QList<QVariant> stored;
	for (int size : ui.splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(TopSplitterSettings, stored);

	plot_mgr_->setNetworkMonitor(nullptr);
	delete monitor_;
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
	QString title = "Plots (" + QString::number(count_) + ")";
	newTabWithName(title);
}

PlotContainer* XeroDashWindow::newTabWithName(QString title)
{
	PlotContainer* cnt = new PlotContainer(*plot_mgr_);
	cnt->setUnits(units_);
	int index = ui.graphs_->addTab(cnt, title);
	QWidget* widget = ui.graphs_->widget(index);
	widget->setProperty(ContainerPropName, QVariant(count_));
	containers_[count_++] = cnt;

	return cnt;
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

	plot_mgr_->setNetworkMonitor(nullptr);
	delete monitor_;
	monitor_ = new NetworkTableMonitor(collection_, ipaddr_.toStdString(), table_name_.toStdString());
	plot_mgr_->setNetworkMonitor(monitor_);

	for (size_t i = 0; i < ui.graphs_->count(); i++)
	{
		QWidget* widget = ui.graphs_->widget(i);
		QVariant v = widget->property(ContainerPropName);
		int index = v.toInt();
		PlotContainer* cnt = containers_[index];
		cnt->setUnits(units_);
	}
}

PlotContainer* XeroDashWindow::currentContainer()
{
	int index = ui.graphs_->currentIndex();
	QWidget* widget = ui.graphs_->widget(index);
	QVariant v = widget->property(ContainerPropName);
	int tag = v.toInt();

	auto it = containers_.find(tag);
	if (it == containers_.end())
		return nullptr;

	return it->second;
}

void XeroDashWindow::closeTab(int which)
{
	QWidget* widget = ui.graphs_->widget(which);
	QVariant v = widget->property(ContainerPropName);
	int tag = v.toInt();
	ui.graphs_->removeTab(which);

	auto it = containers_.find(tag);
	if (it != containers_.end())
	{
		PlotContainer* cnt = containers_[tag];
		delete cnt;
		containers_.erase(it);
	}
}

void XeroDashWindow::editTab(int which)
{
	which_tab_ = which;

	QTabBar* bar = ui.graphs_->tabBar();

	QRect r = bar->tabRect(which);

	if (editor_ == nullptr)
	{
		editor_ = new TabEditName(bar);
		(void)connect(editor_, &TabEditName::returnPressed, this, &XeroDashWindow::editTabDone);
		(void)connect(editor_, &TabEditName::escapePressed, this, &XeroDashWindow::editTabAborted);
	}

	editor_->setGeometry(r);
	editor_->setFocus(Qt::FocusReason::OtherFocusReason);
	editor_->setVisible(true);
	editor_->setText(bar->tabText(which));
	editor_->selectAll();
}

void XeroDashWindow::editTabDone()
{
	QTabBar* bar = ui.graphs_->tabBar();
	QString txt = editor_->text();

	editor_->setVisible(false);
	bar->setTabText(which_tab_, txt);
}

void XeroDashWindow::editTabAborted()
{
	editor_->setVisible(false);
}

int XeroDashWindow::findIndexByTag(int tag)
{
	for (int i = 0; i < ui.graphs_->count(); i++)
	{
		QVariant v = ui.graphs_->widget(i)->property(ContainerPropName);
		if (v.toInt() == tag)
			return i;
	}

	return -1;
}

QJsonDocument XeroDashWindow::createDocument()
{
	QJsonDocument doc;
	QJsonArray arr;

	for(auto pair : containers_)
	{
		PlotContainer* cnt = pair.second;
		QJsonArray plots = cnt->createJSONDescriptor();
		QJsonObject obj;

		int index = findIndexByTag(pair.first);
		obj["name"] = ui.graphs_->tabText(index);
		obj["plots"] = plots;
		arr.append(obj);
	}

	doc.setArray(arr);

	return doc;
}
void XeroDashWindow::fileExit()
{
	close();
}

void XeroDashWindow::helpAbout()
{
	AboutDialog about;
	about.exec();
}


void XeroDashWindow::helpKeyboard()
{
	KeyHints hints;
	hints.exec();
}

void XeroDashWindow::fileLoadLayout()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Layout File"), "", tr("Layout File (*.layout);;All Files (*)"));
	if (filename.length() == 0)
		return;

	QFileInfo info(filename);
	if (!info.exists())
	{
		std::string msg = "File'";
		msg += filename.toStdString();
		msg += "' does not exist";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	QFile file(filename);
	QString text;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::string msg = "File'";
		msg += filename.toStdString();
		msg += "' - cannot open for reading";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	text = file.readAll();
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
	if (doc.isNull())
	{
		std::string msg = "File'";
		msg += filename.toStdString();
		msg += "' - is not a valid layout file";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	if (!doc.isArray())
	{
		std::string msg = "File'";
		msg += filename.toStdString();
		msg += "' - is not a valid layout file";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
		return ;
	}

	while (ui.graphs_->count() > 0)
		closeTab(0);

	QJsonArray arr = doc.array();
	for (QJsonValue v : arr)
	{
		if (v.isObject())
		{
			QJsonObject contobj = v.toObject();
			QString title = contobj["name"].toString();
			PlotContainer *cnt = newTabWithName(title);

			QJsonValue varr = contobj["plots"];
			if (varr.isArray())
				cnt->init(varr.toArray());
		}
	}
}

void XeroDashWindow::fileSaveLayout()
{
	QFileDialog dialog;

	QJsonDocument doc = createDocument();

	QString filename = QFileDialog::getSaveFileName(this, tr("Layout File"), "", tr("Layout File (*.layout);;All Files (*)"));
	if (filename.length() == 0)
		return;

	QFile file(filename);
	if (!file.open(QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::WriteOnly))
		return ;

	file.write(doc.toJson());
	file.close();
}

void XeroDashWindow::editGraphTitle()
{
	currentContainer()->editTitle();
}