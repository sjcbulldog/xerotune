#pragma once

#include <PlotDescriptor.h>
#include <NetworkTableMonitor.h>
#include <QObject>
#include <QListWidget>
#include <list>
#include <mutex>
#include <memory>

class PlotManager : public QObject
{
	Q_OBJECT

public:
	PlotManager(NetworkTableMonitor &monitor, QListWidget &plots, QListWidget &nodes);
	virtual ~PlotManager();

	void tick();

	std::shared_ptr<PlotDescriptor> current() {
		return current_;
	}

private:
	std::shared_ptr<PlotDescriptor> getUpdatedPlotDesc();
	void update(std::shared_ptr<PlotDescriptor> desc);
	
	void itemChanged(QListWidgetItem* item);

private:
	std::mutex lock_;
	NetworkTableMonitor& monitor_;
	QListWidget& plots_;
	QListWidget& nodes_;
	std::shared_ptr<PlotDescriptor> current_;
	std::list<std::shared_ptr<PlotDescriptor>> update_list_;
};

