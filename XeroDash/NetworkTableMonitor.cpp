#include "NetworkTableMonitor.h"
#include "PlotCollection.h"
#include <networktables/EntryListenerFlags.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <iostream>

NetworkTableMonitor::NetworkTableMonitor(PlotCollection& coll, const std::string &ipaddr, const std::string &name) : collection_(coll)
{
	ipaddr_ = ipaddr;
	table_name_ = name;

	inst_ = nt::NetworkTableInstance::GetDefault();
	std::function<void(const nt::EntryNotification & ev)> cb = std::bind(&NetworkTableMonitor::update, this, std::placeholders::_1);
	listener_ = inst_.AddEntryListener(table_name_, cb, 0xffffffff);
	inst_.StartClient(ipaddr_.c_str());
}

NetworkTableMonitor::~NetworkTableMonitor()
{
	inst_ = nt::NetworkTableInstance::GetDefault();
	inst_.RemoveEntryListener(listener_);
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
	auto desc = findOrCreatePlot(plotname.toStdString());

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
	auto desc = findOrCreatePlot(plotname.toStdString());
	desc->clearColumnsAndData();
	for (auto& str : names)
		desc->addColumn(str);

	reset_data_plots_.push_back(desc);
	desc->signalDataReset();
}

void NetworkTableMonitor::setPoints(QString plotname, QString key, int points)
{
	//auto desc = findOrCreatePlot(plotname.toStdString());
	//if (points < desc->dataSize())
	//{
	//	desc->resize(0);
	//	reset_data_plots_.push_back(desc);
	//	desc->signalDataReset();
	//}
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

		qDebug() << "data key added";

		int index = keyword.toInt(&ok);
		if (ok)
		{
			int pl = key.lastIndexOf('/');
			addData(parent, key.mid(0, pl));
		}
	}
	else if (keyword == "points" && notify.value->IsDouble())
	{
		qDebug() << "points key added";

		int count = static_cast<int>(notify.value->GetDouble() + 0.5);
		setPoints(plotname, key, count);
	}
	else if (keyword == "columns" && notify.value->IsStringArray())
	{
		qDebug() << "columns key added";
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

	std::this_thread::sleep_for(std::chrono::microseconds(10));
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

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::getAddedDataPlotDesc()
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

std::shared_ptr<PlotDescriptor> NetworkTableMonitor::findOrCreatePlot(const std::string& name)
{
	auto desc = collection_.findPlot(name);
	if (desc != nullptr)
		return desc;

	desc = collection_.createPlot(name);
	new_plots_.push_back(desc);
	return desc;
}