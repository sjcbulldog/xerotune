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

	return nullptr;
}

void NetworkTableMonitor::addedKey(const nt::EntryNotification& notify)
{
	std::lock_guard guard(key_list_lock_);
	QString qname(notify.name.c_str());
	QStringList words = qname.split('/');
	int index;
	bool ok;

	if (words.size() <= 2)
		return;

	QString keyword = words.back();
	words.pop_back();

	QString plotname = words.back();
	words.pop_back();

	QString parent = words.back();
	words.pop_back();

	index = keyword.toInt(&ok);
	if (ok && plotname == "data")
	{
		if (notify.value->IsDoubleArray())
		{
			auto desc = findPlot(parent);
			if (desc == nullptr)
			{
				QListWidgetItem* item = new QListWidgetItem(parent);
				desc = std::make_shared<PlotDescriptor>(item);
				new_plots_.push_back(desc);
				all_plots_.push_back(desc);
			}

			if (desc->getIndex() < index) {
				qDebug() << "key data - desc index " << desc->getIndex() << " index " << index;
				//
				// We might be coming in the middle, see if other data is already in
				// the network table
				//
				QString loc = notify.name.c_str();
				int pl = loc.lastIndexOf('/') ;
				getEarlierData(loc.mid(0, pl), desc, index);
			}

			auto& ndata = notify.value->GetDoubleArray();
			desc->addData(index, ndata);
		}
	}
	else if (keyword == "inited")
	{
		if (notify.value->IsBoolean())
		{
			std::shared_ptr<PlotDescriptor> plot = findPlot(plotname);
			if (plot == nullptr)
			{
				//
				// This is a new plot showing up.  Lets create the descriptor for the new plot
				// and store in the list to be processed
				//
				QListWidgetItem* item = new QListWidgetItem(plotname);
				plot = std::make_shared<PlotDescriptor>(item);
				new_plots_.push_back(plot);
				all_plots_.push_back(plot);
			}

			plot->setInited(notify.value->GetBoolean());
		}
	}
	else if (keyword == "active")
	{
		if (notify.value->IsBoolean())
		{
			auto desc = findPlot(plotname);
			if (desc == nullptr)
			{
				QListWidgetItem* item = new QListWidgetItem(parent);
				desc = std::make_shared<PlotDescriptor>(item);
				new_plots_.push_back(desc);
				all_plots_.push_back(desc);
			}

			desc->setActive(notify.value->GetBoolean());
		}
	}
	else if (keyword == "columns")
	{
		if (notify.value->IsStringArray())
		{
			auto desc = findPlot(plotname);
			if (desc == nullptr)
			{
				QListWidgetItem* item = new QListWidgetItem(parent);
				desc = std::make_shared<PlotDescriptor>(item);
				new_plots_.push_back(desc);
				all_plots_.push_back(desc);
			}

			desc->clearColumns();
			for (auto& str : notify.value->GetStringArray())
				desc->addColumn(str);
		}
	}
}

void NetworkTableMonitor::getEarlierData(QString loc, std::shared_ptr<PlotDescriptor> desc, int index)
{
	nt::NetworkTableInstance nt = nt::NetworkTableInstance::GetDefault();
	auto table = nt.GetTable(loc.toStdString());

	int here = desc->getIndex();

	while (here < index)
	{
		QString key = QString::number(here);
		auto value = table->GetValue(key.toStdString());

		if (value->IsDoubleArray())
			desc->addData(here, value->GetDoubleArray());

		here++;
	}
}

void NetworkTableMonitor::updatedKey(const nt::EntryNotification& notify)
{
	std::lock_guard guard(key_list_lock_);
	QString qname(notify.name.c_str());
	QStringList words = qname.split('/');
	bool ok;
	int index;

	if (words.size() <= 2)
		return;

	QString keyword = words.back();
	words.pop_back();

	QString plotname = words.back();
	words.pop_back();

	QString parent = words.back();
	words.pop_back();

	index = keyword.toInt(&ok);
	if (ok && plotname == "data")
	{
		if (notify.value->IsDoubleArray())
		{
			auto desc = findPlot(parent);

			if (desc->getIndex() < index) {
				//
				// We might be coming in the middle, see if other data is already in
				// the network table
				//
				QString loc = notify.name.c_str();
				int pl = loc.lastIndexOf('/') ;
				loc.chop(pl);
				getEarlierData(loc, desc, index);
			}

			auto& ndata = notify.value->GetDoubleArray();
			desc->addData(index, ndata);
		}
	}

	if (keyword == "inited")
	{
		if (notify.value->IsBoolean())
		{
			auto desc = findPlot(plotname);
			if (desc != nullptr)
				desc->setInited(notify.value->GetBoolean());
		}
	}
	else if (keyword == "active")
	{
		if (notify.value->IsBoolean())
		{
			auto desc = findPlot(plotname);
			if (desc != nullptr)
				desc->setActive(notify.value->GetBoolean());
		}
	}
	else if (keyword == "columns")
	{
		if (notify.value->IsStringArray())
		{
			auto desc = findPlot(plotname);

			desc->clearColumns();

			for (auto& str : notify.value->GetStringArray())
				desc->addColumn(str);
		}
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