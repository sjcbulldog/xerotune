#pragma once

#include <QString>
#include <memory>

class PlotDescriptor;

class CSVToPlotDescriptor
{
public:
	CSVToPlotDescriptor() = delete;
	~CSVToPlotDescriptor() = delete;

	static std::shared_ptr<PlotDescriptor> loadPlotData(const QString& filename);

private:
};

