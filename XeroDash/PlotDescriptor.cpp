#include "PlotDescriptor.h"
#include <QDebug>
#include <fstream>

PlotDescriptor::PlotDescriptor(QListWidgetItem *item)
{
	item_ = item;
	inited_ = false;
	active_ = false;
	consolidated_ = false;
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
		valid_.clear();
		consolidated_ = false;
	}

	active_ = b;
	emitActiveChanged();
}

void PlotDescriptor::enable()
{
	active_ = true;
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

void PlotDescriptor::emitDataCompleted()
{
	emit dataCompleted();
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

void PlotDescriptor::consolidate()
{
	size_t total = valid_.size();

	std::list<size_t> missing;

	for (size_t i = 0; i < valid_.size(); i++)
	{
		if (!valid_[i])
			missing.push_back(i);
	}

	percent_ = (double)(valid_.size() - missing.size()) / (double)(valid_.size()) * 100.0;

	while (missing.size() > 0)
	{
		size_t item = missing.back();
		missing.pop_back();

		auto vit = valid_.begin();
		std::advance(vit, item);
		valid_.erase(vit);

		auto dit = data_.begin();
		std::advance(dit, item);
		data_.erase(dit);
	}

	QString msg = QString::number(total) + " sample total, ";
	msg += QString::number(percent_, 'f', 1) + "% captured";
	item_->setToolTip(msg);

	if (percent_ < 80.0)
		item_->setBackgroundColor(QColor(0xff, 0xc0, 0xc0, 0xfF));

	consolidated_ = true;
	emitDataCompleted();
}

void PlotDescriptor::addData(int index, const std::vector<double>& data)
{
	if (!consolidated_)
	{
		if (data_.size() <= index)
		{
			size_t old = valid_.size();

			data_.resize(index + 1);
			valid_.resize(index + 1);
			for (size_t i = old; i < valid_.size(); i++)
				valid_[i] = false;
		}

		data_[index] = data;
		valid_[index] = true;
	}
	else
	{
		assert(false);
	}
}
