#pragma once

#include "PlotDescriptor.h"
#include "PlotManager.h"
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QString>

class SingleChart : public QtCharts::QChartView
{
	Q_OBJECT

public:
	SingleChart(PlotManager& mgr, QWidget *parent = Q_NULLPTR);
	~SingleChart();

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dragMoveEvent(QDragMoveEvent* event) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
	virtual void dropEvent(QDropEvent* ev) override;

private:
	void insertNode(QString node);
	QtCharts::QValueAxis* findAxis(QString axisname);
	QtCharts::QLineSeries* findSeries(QString node);
	QString nodeToAxis(QString onde);

private:
	PlotManager& plot_mgr_;
	std::shared_ptr<PlotDescriptor> desc_;
	QtCharts::QValueAxis* time_;
};
