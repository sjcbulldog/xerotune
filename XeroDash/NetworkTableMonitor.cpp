#include "NetworkTableMonitor.h"
#include <networktables/EntryListenerFlags.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <iostream>

NetworkTableMonitor* NetworkTableMonitor::theOne = nullptr;

NetworkTableMonitor::NetworkTableMonitor()
{
	assert(theOne == nullptr);
	theOne = this;
	running_ = false;
	stopped_ = true;
	last_was_data_ = false;
}

NetworkTableMonitor::~NetworkTableMonitor()
{
	stop(1000 * 10);
	assert(theOne == this);
	theOne = nullptr;
}

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::itemToDesc(QListWidgetItem* item)
{
	std::lock_guard guard(key_list_lock_);

	for (auto plot : all_plots_)
	{
		if (plot->item() == item)
			return plot;
	}

	return nullptr;
}

bool NetworkTableMonitor::setIPAddress(const std::string& addr)
{
	std::lock_guard guard(thread_lock_);

	if (running_)
		return false;

	ipaddr_ = addr;
	return true;
}

bool NetworkTableMonitor::start(const std::string& name)
{
	std::lock_guard guard(thread_lock_);

	if (running_)
		return false;

	table_name_ = name;

	running_ = true;
	thread_ = std::thread(&NetworkTableMonitor::monitorThread, this);
	return true;
}

bool NetworkTableMonitor::stop(int waitms)
{
	std::chrono::milliseconds delay(1);
	std::lock_guard guard(thread_lock_);

	inst_ = nt::NetworkTableInstance::GetDefault();
	inst_.RemoveEntryListener(listener_);

	stopped_ = false;
	running_ = false;

	while (!stopped_ && waitms > 0) {
		std::this_thread::sleep_for(delay);
		waitms--;
	}

	if (stopped_ && thread_.joinable())
		thread_.join();


	return stopped_;
}

void NetworkTableMonitor::instCallback(const nt::EntryNotification &notify)
{
	if (theOne == nullptr)
		return;

	theOne->update(notify);
}

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::findPlot(QString name)
{
	std::lock_guard guard(thread_lock_);

	for (auto desc : all_plots_)
	{
		if (desc->name() == name)
			return desc;
	}

	QListWidgetItem* item = new QListWidgetItem(name);
	std::shared_ptr<PlotDescriptor> desc = std::make_shared<PlotDescriptor>(item);
	new_plots_.push_back(desc);
	all_plots_.push_back(desc);

	return desc;
}

void NetworkTableMonitor::addData(QString plotname, QString datakey)
{
	auto desc = findPlot(plotname);

	nt::NetworkTableInstance nt = nt::NetworkTableInstance::GetDefault();
	auto table = nt.GetTable(datakey.toStdString());

	int here = 0;
	int endval;
	auto value = table->GetValue("points");

	if (value == nullptr || !value->IsDouble())
		return;

	endval = static_cast<int>(value->GetDouble() + 0.5);
	while (here < endval)
	{
		if (!desc->hasData(here))
		{
			QString key = "data/" + QString::number(here);
			auto value = table->GetValue(key.toStdString());
			if (value != nullptr && value->IsDoubleArray())
				desc->addData(here, value->GetDoubleArray());
		}
		here++;
	}
}

void NetworkTableMonitor::setInited(QString plotname, QString key, bool b)
{
	last_was_data_ = false;

	std::shared_ptr<PlotDescriptor> plot = findPlot(plotname);
	plot->setInited(b);

	if (b)
		addData(plotname, key);
}

void NetworkTableMonitor::setActive(QString plotname, QString key, bool b)
{
	last_was_data_ = false;

	auto desc = findPlot(plotname);
	desc->setActive(b);

	addData(plotname, key);

	if (!b && desc->hasData())
		desc->consolidate();
}

void NetworkTableMonitor::setColumns(QString plotname, QString key, const wpi::ArrayRef<std::string>& names)
{
	last_was_data_ = false;

	auto desc = findPlot(plotname);

	desc->clearColumns();
	for (auto& str : names)
		desc->addColumn(str);
}

void NetworkTableMonitor::addedKey(const nt::EntryNotification& notify)
{
	std::lock_guard guard(key_list_lock_);
	QString qname(notify.name.c_str());
	QStringList words = qname.split('/');
	int pl = qname.lastIndexOf('/');
	QString key = qname.mid(0, pl);

	if (words.size() <= 2)
		return;

	QString keyword = words.back();
	words.pop_back();

	QString plotname = words.back();
	words.pop_back();

	QString parent = words.back();
	words.pop_back();

	if (plotname == "data" && notify.value->IsDoubleArray())
	{
		bool ok;

		int index = keyword.toInt(&ok);
		if (ok)
		{
			int pl = key.lastIndexOf('/');
			addData(parent, key.mid(0, pl));
		}
	}
	else if (keyword == "inited" && notify.value->IsBoolean())
	{
		setInited(plotname, key, notify.value->GetBoolean());
	}
	else if (keyword == "active" && notify.value->IsBoolean())
	{
		setActive(plotname, key, notify.value->GetBoolean());
	}
	else if (keyword == "columns" && notify.value->IsStringArray())
	{
		setColumns(plotname, key, notify.value->GetStringArray());
	}
}

void NetworkTableMonitor::updatedKey(const nt::EntryNotification& notify)
{
	std::lock_guard guard(key_list_lock_);
	QString qname(notify.name.c_str());
	QStringList words = qname.split('/');
	int pl = qname.lastIndexOf('/');
	QString key = qname.mid(0, pl);

	if (words.size() <= 2)
		return;

	QString keyword = words.back();
	words.pop_back();

	QString plotname = words.back();
	words.pop_back();

	QString parent = words.back();
	words.pop_back();

	if (plotname == "data" && notify.value->IsDoubleArray())
	{
		bool ok;

		int index = keyword.toInt(&ok);
		if (ok)
			addData(parent, key);
	}

	if (keyword == "inited" && notify.value->IsBoolean())
	{
		setInited(plotname, key, notify.value->GetBoolean());
	}
	else if (keyword == "active" && notify.value->IsBoolean())
	{
		setActive(plotname, key, notify.value->GetBoolean());
	}
	else if (keyword == "columns" && notify.value->IsStringArray())
	{
		setColumns(plotname, key, notify.value->GetStringArray());
	}
}

void NetworkTableMonitor::deletedKey(const nt::EntryNotification& notify)
{
	std::lock_guard guard(key_list_lock_);
	QString qname(notify.name.c_str());
	QStringList words = qname.split('/');

	if (words.size() <= 2)
		return;

	QString keyword = words.back();
	words.pop_back();

	QString plotname = words.back();
	words.pop_back();
}

void NetworkTableMonitor::update(const nt::EntryNotification& notify)
{
	if ((notify.flags & NT_NOTIFY_NEW) == NT_NOTIFY_NEW)
		addedKey(notify);
	else if ((notify.flags & NT_NOTIFY_UPDATE) == NT_NOTIFY_UPDATE)
		updatedKey(notify);
	else if ((notify.flags & NT_NOTIFY_DELETE) == NT_NOTIFY_DELETE)
		deletedKey(notify);
}

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::getPlotDesc()
{
	std::shared_ptr<PlotDescriptor> desc;
	std::lock_guard guard(key_list_lock_);

	if (new_plots_.size() > 0)
	{
		desc = new_plots_.front();
		new_plots_.pop_front();
	}

	return desc;
}

void NetworkTableMonitor::monitorThread()
{
	std::chrono::milliseconds delay(1000);
	int flags;

	inst_ = nt::NetworkTableInstance::GetDefault();
	listener_ = inst_.AddEntryListener(table_name_, instCallback, 0xffffffff);
	inst_.StartClient(ipaddr_.c_str());

	while (running_)
	{
		std::this_thread::sleep_for(delay);
	}

	stopped_ = true;
}