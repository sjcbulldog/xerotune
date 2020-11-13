#pragma once

#include <PlotDescriptor.h>
#include <NetworkTableMonitor.h>
#include <QtCore/QObject>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLabel>
#include <list>
#include <mutex>
#include <memory>
#include <chrono>

class PlotCollection;

class PlotManager : public QObject
{
	Q_OBJECT

public:
	PlotManager(PlotCollection &coll, QListWidget &plots, QListWidget &nodes);
	virtual ~PlotManager();

	void tick();

	std::shared_ptr<PlotDescriptor> current() {
		return current_;
	}

	std::shared_ptr<PlotDescriptor> find(QString name);

	void setNetworkMonitor(NetworkTableMonitor* monitor) {
		monitor_ = monitor;
	}

	void setStatusLabel(QLabel* label) {
		status_ = label;
	}

signals:
	void newPlot();

private:
	void selectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void emitNewPlot();
	void removeDataAddedList(std::shared_ptr<PlotDescriptor> desc);
	std::shared_ptr<PlotDescriptor> findLoadedData(const QString& name);

private:
	std::mutex lock_;
	NetworkTableMonitor *monitor_;
	QListWidget& plots_;
	QListWidget& nodes_;
	std::shared_ptr<PlotDescriptor> current_;
	int64_t age_threshold_;

	std::list<std::shared_ptr<PlotDescriptor>> plot_data_;

	std::list<std::pair<std::chrono::high_resolution_clock::time_point, std::shared_ptr<PlotDescriptor>>> data_added_list_;

	PlotCollection& collection_;

	QLabel* status_;
};

