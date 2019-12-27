#include "PlotCollection.h"
#include "PlotDescriptor.h"
#include <cassert>

PlotCollection::PlotCollection()
{
}

PlotCollection::~PlotCollection()
{
}

std::shared_ptr<PlotDescriptor> PlotCollection::createPlot(const std::string& name)
{
	assert(plots_.find(name) == plots_.end());

	auto desc = std::make_shared<PlotDescriptor>(name);
	plots_[name] = desc;
	return desc;
}

std::shared_ptr<PlotDescriptor> PlotCollection::findPlot(const std::string& name)
{
	auto it = plots_.find(name);
	if (it == plots_.end())
		return nullptr;

	return it->second;
}

bool PlotCollection::loadPlotData(const std::string &name)
{
	return false;
}

bool PlotCollection::savePlotData(const std::string& name)
{
	return false;
}