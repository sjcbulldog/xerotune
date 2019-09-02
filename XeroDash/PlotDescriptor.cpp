#include "PlotDescriptor.h"
#include <QDebug>
#include <fstream>

PlotDescriptor::PlotDescriptor(QListWidgetItem *item)
{
	item_ = item;
	inited_ = false;
	active_ = false;
	index_ = 0;
	total_lost_ = 0;
}

PlotDescriptor::~PlotDescriptor()
{
}

void PlotDescriptor::emitActiveChanged()
{
	emit activeChanged();
}

void PlotDescriptor::emitInitedChanged()
{
	emit initedChanged();
}

void PlotDescriptor::emitDataAdded()
{
	emit dataAdded();
}

size_t PlotDescriptor::getColumnIndexFromName(const std::string& name) const
{
	for (size_t i = 0; i < columns_.size(); i++)
	{
		if (columns_[i] == name)
			return i;
	}

	return std::numeric_limits<size_t>::max();
}

void PlotDescriptor::addData(int index, const std::vector<double>& data)
{
	if (index != index_)
	{
		total_lost_ += (index - index_);
		qDebug() << "lost data: expected " << index_ << ", got " << index << 
						", " << total_lost_ << "total lost";
		index_ = index + 1;
	}
	else
		index_++;

	data_.push_back(data);
}
