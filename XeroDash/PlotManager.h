#pragma once

#include <PlotDescriptor.h>
#include <NetworkTableMonitor.h>
#include <QObject>
#include <QListWidget>
#include <list>
#include <mutex>
#include <memory>
#include <chrono>

class PlotManager : public QObject
{
	Q_OBJECT

public:
	PlotManager(QListWidget &plots, QListWidget &nodes);
	virtual ~PlotManager();

	void tick();

	std::shared_ptr<PlotDescriptor> current() {
		return current_;
	}

	std::shared_ptr<PlotDescriptor> find(QString name);

	void setNetworkMonitor(NetworkTableMonitor* monitor) {
		monitor_ = monitor;
	}

signals:
	void newPlot();

private:
	void selectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void emitNewPlot();
	void removeDataAddedList(std::shared_ptr<PlotDescriptor> desc);

private:
	std::mutex lock_;
	NetworkTableMonitor *monitor_;
	QListWidget& plots_;
	QListWidget& nodes_;
	std::shared_ptr<PlotDescriptor> current_;
	int64_t age_threshold_;

	std::list<std::pair<std::chrono::high_resolution_clock::time_point, std::shared_ptr<PlotDescriptor>>> data_added_list_;
};

