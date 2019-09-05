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

class NetworkTableMonitor
{
public:
	NetworkTableMonitor();
	virtual ~NetworkTableMonitor();

	bool setIPAddress(const std::string& addr);
	bool start(const std::string &name);
	bool stop(int waitms);

	std::shared_ptr<PlotDescriptor> findPlot(QString name);

	std::shared_ptr<PlotDescriptor> getPlotDesc();

	std::shared_ptr<PlotDescriptor> itemToDesc(QListWidgetItem* item);

private:
	static void instCallback(const nt::EntryNotification& notify);

	void addedKey(const nt::EntryNotification& notify);
	void updatedKey(const nt::EntryNotification& notify);
	void deletedKey(const nt::EntryNotification& notify);

	void monitorThread();
	void update(const nt::EntryNotification& notify);

	void addDataToPlot();

	void addData(QString plotname, QString key);
	void setInited(QString plotname, QString key, bool value);
	void setActive(QString plotname, QString key, bool value);
	void setColumns(QString plotname, QString key, const wpi::ArrayRef<std::string>& names);

private:
	std::string table_name_;
	std::mutex thread_lock_;
	std::mutex key_list_lock_;
	std::string ipaddr_;
	std::thread thread_;
	bool running_;
	bool stopped_;
	nt::NetworkTableInstance inst_;
	std::list<std::shared_ptr<PlotDescriptor>> all_plots_;
	std::list<std::shared_ptr<PlotDescriptor>> new_plots_;
	NT_EntryListener listener_;
	bool last_was_data_;

	static NetworkTableMonitor* theOne;
};

