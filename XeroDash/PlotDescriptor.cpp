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

void PlotDescriptor::setActive(bool b)
{
	if (!active_ && b)
	{
		//
		// We are transitioning from not active to active, clear data and reset index counter
		//
		data_.clear();
		index_ = 0;
		total_lost_ = 0;
	}

	active_ = b;
	emitActiveChanged();
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
	if (index < index_)
		return ;

	if (index != index_)
	{
		total_lost_ += (index - index_);
		qDebug() << "lost data: expected " << index_ << ", got " << index << 
						", " << total_lost_ << "total lost";
		index_ = index + 1;
	}
	else
		index_++;

	//
// We are transitioning from active to not active, we are done with data for this round
//
	if (percentCaptured() < 90.0)
		item_->setBackgroundColor(QColor(0xff, 0xc0, 0xc0, 0xFF));
	else
		item_->setBackgroundColor(QColor(0x0ff, 0x0ff, 0xff, 0xff));

	QString tip;

	tip = QString::number(totalSamples()) + " samples, ";
	tip += QString::number(percentCaptured(), 'f', 0) + "% captured";
	item_->setToolTip(tip);

	data_.push_back(data);
	emitDataAdded();

}
