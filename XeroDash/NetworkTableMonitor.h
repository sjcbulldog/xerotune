#pragma once

#include "PlotDescriptor.h"
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <thread>
#include <mutex>
#include <string>
#include <list>
#include <map>
#include <memory>

class PlotCollection;

class NetworkTableMonitor
{
public:
	NetworkTableMonitor(PlotCollection &coll, const std::string &ipaddr, const std::string& name);
	virtual ~NetworkTableMonitor();

	std::shared_ptr<PlotDescriptor> findPlot(QString name, bool create = true);

	std::shared_ptr<PlotDescriptor> getNewPlotDesc();
	std::shared_ptr<PlotDescriptor> getAddedDataPlotDesc();
	std::shared_ptr<PlotDescriptor> getResetPlotDesc();

private:
	void addedKey(const nt::EntryNotification& notify);
	void updatedKey(const nt::EntryNotification& notify);
	void deletedKey(const nt::EntryNotification& notify);

	void update(const nt::EntryNotification& notify);

	void addData(QString plotname, QString key);
	void setColumns(QString plotname, QString key, const wpi::ArrayRef<std::string>& names);
	void setPoints(QString plotsname, QString key, int points);

	std::shared_ptr<PlotDescriptor> findOrCreatePlot(const std::string& name);

private:
	std::string table_name_;
	std::mutex key_list_lock_;
	std::string ipaddr_;
	std::thread thread_;
	nt::NetworkTableInstance inst_;

	std::list<std::shared_ptr<PlotDescriptor>> new_plots_;
	std::list<std::shared_ptr<PlotDescriptor>> added_data_plots_;
	std::list<std::shared_ptr<PlotDescriptor>> reset_data_plots_;

	NT_EntryListener listener_;

	PlotCollection& collection_;
};

