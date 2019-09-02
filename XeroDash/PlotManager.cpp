#include "PlotManager.h"

PlotManager::PlotManager(NetworkTableMonitor& monitor, QListWidget& plots, QListWidget &nodes) 
				: monitor_(monitor), plots_(plots), nodes_(nodes)
{
	(void)connect(&plots_, &QListWidget::itemChanged, this, &PlotManager::itemChanged);

	nodes_.setSelectionMode(QAbstractItemView::SingleSelection);
	nodes_.setDragEnabled(true);
	nodes_.setDragDropMode(QAbstractItemView::DragDrop);

	current_ = nullptr;
}

PlotManager::~PlotManager()
{

}

void PlotManager::update(std::shared_ptr<PlotDescriptor> desc)
{
	std::lock_guard<std::mutex> guard(lock_);
	update_list_.push_back(desc);
}

std::shared_ptr<PlotDescriptor> PlotManager::getUpdatedPlotDesc()
{
	std::shared_ptr<PlotDescriptor> desc;
	std::lock_guard<std::mutex> guard(lock_);

	if (update_list_.size() > 0)
	{
		desc = update_list_.front();
		update_list_.pop_front();
	}

	return desc;
}

void PlotManager::tick()
{
	std::shared_ptr<PlotDescriptor> desc;

	//
	// Check for new plot descriptors
	//
	while (true)
	{
		desc = monitor_.getPlotDesc();
		if (desc == nullptr)
			break;

		QListWidgetItem* item = desc->item();
		item->setFlags(Qt::NoItemFlags);
		plots_.addItem(item);

		(void)connect(desc.get(), &PlotDescriptor::activeChanged, this, [desc, this]() { this->update(desc); });
		(void)connect(desc.get(), &PlotDescriptor::initedChanged, this, [desc, this]() { this->update(desc); });
		(void)connect(desc.get(), &PlotDescriptor::dataAdded, this, [desc, this]() { this->update(desc); });
	}

	//
	// Check for updated plot descriptors
	//
	while (true)
	{
		desc = getUpdatedPlotDesc();
		if (desc == nullptr)
			break;

		QListWidgetItem* item = desc->item();
		if (item == nullptr)
			return;

		if (desc->inited())
		{
			item->font().setBold(true);
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

			if (desc->active())
			{
				item->setForeground(QBrush(QColor(0x00, 0x00, 0xff, 0xFF)));
				plots_.repaint();
			}
			else
			{
				item->setForeground(QBrush(QColor(0x00, 0x00, 0x00, 0xFF)));
				plots_.repaint();
			}

			if (desc->dataDirty())
			{
			}
		}
		else
		{
			desc->item()->setFlags(Qt::NoItemFlags);
			desc->item()->setForeground(QBrush(QColor(0x00, 0x00, 0x00, 0xFF)));
		}
	}
}

void PlotManager::itemChanged(QListWidgetItem* item)
{
	nodes_.clear();
	current_ = monitor_.itemToDesc(item);

	if (current_ != nullptr && current_->hasColumns())
	{
		for (const std::string& col : current_->columns())
		{
			QListWidgetItem* item = new QListWidgetItem(col.c_str());
			nodes_.addItem(item);
		}
	}
}