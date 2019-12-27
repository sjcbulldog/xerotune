#pragma once

#include <string>
#include <memory>
#include <map>
#include <mutex>

class PlotDescriptor;

class PlotCollection
{
public:
	PlotCollection();
	virtual ~PlotCollection();

	std::shared_ptr<PlotDescriptor> createPlot(const std::string& name);
	std::shared_ptr<PlotDescriptor> findPlot(const std::string& name);

	bool loadPlotData(const std::string& filename);
	bool savePlotData(const std::string& filename);

private:
	std::mutex plot_lock_;
	std::map<std::string, std::shared_ptr<PlotDescriptor>> plots_;
};

