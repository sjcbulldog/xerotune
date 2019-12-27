#include "PlotManager.h"
#include "PlotCollection.h"
#include <QDebug>

typedef std::pair<std::chrono::high_resolution_clock::time_point, std::shared_ptr<PlotDescriptor>> pairitem;

PlotManager::PlotManager(PlotCollection& coll, QListWidget& plots, QListWidget &nodes) : plots_(plots), nodes_(nodes), collection_(coll)
{
	nodes_.setDragEnabled(true);
	nodes_.setSelectionMode(QAbstractItemView::SingleSelection);
	nodes_.setDragDropMode(QAbstractItemView::DragOnly);

	plots_.setDragEnabled(false);
	plots_.setDragDropMode(QAbstractItemView::NoDragDrop);
	plots_.setSelectionMode(QAbstractItemView::SingleSelection);

	(void)connect(&plots_, &QListWidget::currentItemChanged, this, &PlotManager::selectionChanged);

	current_ = nullptr;
	monitor_ = nullptr;

	age_threshold_ = 100;
}

PlotManager::~PlotManager()
{
}

std::shared_ptr<PlotDescriptor> PlotManager::find(QString name)
{
	return collection_.findPlot(name.toStdString());
}

void PlotManager::tick()
{
	if (monitor_ == nullptr)
		return;

	std::shared_ptr<PlotDescriptor> desc;

	//
	// Check for new plot descriptors
	//
	desc = monitor_->getNewPlotDesc();
	if (desc != nullptr)
	{
		QListWidgetItem* item = new QListWidgetItem(desc->name().c_str());
		plots_.addItem(item);
		emitNewPlot();

		if (plots_.selectedItems().size() == 0)
			item->setSelected(true);
	}

	desc = monitor_->getAddedDataPlotDesc();
	if (desc != nullptr)
		desc->signalDataAdded();

	desc = monitor_->getResetPlotDesc();
	if (desc != nullptr)
		desc->signalDataReset();

	auto now = std::chrono::high_resolution_clock::now();
	while (true)
	{
		desc = monitor_->getAddedDataPlotDesc();
		if (desc == nullptr)
			break;

		removeDataAddedList(desc);
		data_added_list_.push_back(std::make_pair(now, desc));
	}

	//
	// Finally see if there are any data added plots that have not added data in the
	// last age_threshold_ time
	//
	std::list<pairitem> toremove;

	for (auto pair : data_added_list_)
	{
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pair.first);
		if (elapsed.count() > age_threshold_)
		{
			pair.second->signalDataAdded();
			toremove.push_back(pair);
		}
	}

	for (auto item : toremove)
		removeDataAddedList(item.second);
}

void PlotManager::removeDataAddedList(std::shared_ptr<PlotDescriptor> desc)
{
	auto it = std::find_if(data_added_list_.begin(), data_added_list_.end(), [desc](pairitem pair) { return pair.second == desc; });
	if (it != data_added_list_.end())
		data_added_list_.erase(it);
}

//
// This is called in the GUI thread.  This is called when the
// selected plot in the plot window changes.  We need to clear
// the nodes window and repopulate based on the currently selected
// plot
//
void PlotManager::selectionChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	nodes_.clear();

	if (current != nullptr)
	{
		auto desc = collection_.findPlot(current->text().toStdString());
		current_ = desc;
		std::vector<std::string> cols = desc->columns();

		for (size_t i = 1; i < cols.size(); i++)
			nodes_.addItem(cols[i].c_str());

		QString str;
		str = current->text();
		str += ", nodes " + QString::number(desc->columns().size());
		str += ", datapoints " + QString::number(desc->data().size());
		status_->setText(str);
	}
}

void PlotManager::emitNewPlot()
{
	emit newPlot();
}
