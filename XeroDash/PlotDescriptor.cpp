#include "PlotDescriptor.h"
#include <QtCore/QDebug>
#include <fstream>

PlotDescriptor::PlotDescriptor(const std::string &name)
{
	name_ = name;
	inited_ = false;
	active_ = false;
}

PlotDescriptor::~PlotDescriptor()
{
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

void PlotDescriptor::getDataAndValid(std::vector<bool>& valid, std::vector<std::vector<double>>& data)
{
	std::lock_guard guard(lock_);

	data = data_;
	valid = valid_;
}

void PlotDescriptor::addData(size_t index, const std::vector<double>& data)
{
	std::lock_guard guard(lock_);

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

	qDebug() << "PlotDescriptor:: addData size = " << data_.size() << " " << valid_.size();
}

void PlotDescriptor::resize(size_t size)
{
	std::lock_guard guard(lock_);

	if (size < data_.size())
	{
		data_.resize(size);
		valid_.resize(size);
	}
}

void PlotDescriptor::emitDataAdded()
{
	emit dataAdded();
}

void PlotDescriptor::emitDataReset()
{
	emit dataReset();
}
