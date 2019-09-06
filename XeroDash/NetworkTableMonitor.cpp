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

NetworkTableMonitor::NetworkTableMonitor(const std::string &ipaddr, const std::string &name)
{
	assert(theOne == nullptr);
	theOne = this;

	ipaddr_ = ipaddr;
	table_name_ = name;

	inst_ = nt::NetworkTableInstance::GetDefault();
	listener_ = inst_.AddEntryListener(table_name_, instCallback, 0xffffffff);
	inst_.StartClient(ipaddr_.c_str());
}

NetworkTableMonitor::~NetworkTableMonitor()
{
	inst_ = nt::NetworkTableInstance::GetDefault();
	inst_.RemoveEntryListener(listener_);

	assert(theOne == this);
	theOne = nullptr;
}

void NetworkTableMonitor::instCallback(const nt::EntryNotification& notify)
{
	if (theOne == nullptr)
		return;

	theOne->update(notify);
}

//
// Utilities for dealing with finding data
//

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

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::findPlot(QString name, bool create)
{
	std::lock_guard guard(key_list_lock_);
	return findPlotNoLock(name, create);
}

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::findPlotNoLock(QString name, bool create)
{
	std::shared_ptr<PlotDescriptor> desc;

	for (auto it = all_plots_.begin(); it != all_plots_.end(); it++)
	{
		desc = *it;
		if (desc->name() == name)
			return desc;
	}

	if (create)
	{
		QListWidgetItem* item = new QListWidgetItem(name);
		item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
		desc = std::make_shared<PlotDescriptor>(item);
		new_plots_.push_back(desc);
		all_plots_.push_back(desc);
	}

	return desc;
}

//
// Interacting with the network table
//

//
// Extract all data from the network table that might exist for the given
// plot.
//
void NetworkTableMonitor::addData(QString plotname, QString datakey)
{
	auto desc = findPlotNoLock(plotname);

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

	added_data_plots_.push_back(desc);
}

void NetworkTableMonitor::setColumns(QString plotname, QString key, const wpi::ArrayRef<std::string>& names)
{
	auto desc = findPlotNoLock(plotname);

	desc->clearColumns();
	for (auto& str : names)
		desc->addColumn(str);
}

void NetworkTableMonitor::setPoints(QString plotname, QString key, int points)
{
	auto desc = findPlotNoLock(plotname);

	if (points < desc->dataSize())
	{
		desc->resize(0);
		reset_data_plots_.push_back(desc);
	}
}

void NetworkTableMonitor::addedKey(const nt::EntryNotification& notify)
{
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
	else if (keyword == "points" && notify.value->IsDouble())
	{
		int count = static_cast<int>(notify.value->GetDouble() + 0.5);
		setPoints(plotname, key, count);
	}
	else if (keyword == "columns" && notify.value->IsStringArray())
	{
		setColumns(plotname, key, notify.value->GetStringArray());
	}
}

void NetworkTableMonitor::updatedKey(const nt::EntryNotification& notify)
{
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
	else if (keyword == "points" && notify.value->IsDouble())
	{
		int count = static_cast<int>(notify.value->GetDouble() + 0.5);
		setPoints(plotname, key, count);
	}
	else if (keyword == "columns" && notify.value->IsStringArray())
	{
		setColumns(plotname, key, notify.value->GetStringArray());
	}
}

void NetworkTableMonitor::deletedKey(const nt::EntryNotification& notify)
{
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
	std::lock_guard guard(key_list_lock_);

	if ((notify.flags & NT_NOTIFY_NEW) == NT_NOTIFY_NEW)
		addedKey(notify);
	else if ((notify.flags & NT_NOTIFY_UPDATE) == NT_NOTIFY_UPDATE)
		updatedKey(notify);
	else if ((notify.flags & NT_NOTIFY_DELETE) == NT_NOTIFY_DELETE)
		deletedKey(notify);

	std::this_thread::yield();
}

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::getNewPlotDesc()
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

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::getAddedDateaPlotDesc()
{
	std::shared_ptr<PlotDescriptor> desc;
	std::lock_guard guard(key_list_lock_);

	if (added_data_plots_.size() > 0)
	{
		desc = added_data_plots_.front();
		added_data_plots_.pop_front();
	}

	return desc;
}

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::getResetPlotDesc()
{
	std::shared_ptr<PlotDescriptor> desc;
	std::lock_guard guard(key_list_lock_);

	if (reset_data_plots_.size() > 0)
	{
		desc = reset_data_plots_.front();
		reset_data_plots_.pop_front();
	}

	return desc;
}