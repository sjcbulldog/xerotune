#pragma once

#include "PlotManager.h"
#include "SingleChart.h"
#include <QWidget>
#include <QGridLayout>
#include <QJsonArray>
#include <vector>

class PlotContainer : public QWidget
{
	Q_OBJECT

public:
	PlotContainer(PlotManager& mgr, QWidget *parent = Q_NULLPTR);
	~PlotContainer();

	void childFocused(SingleChart*);
	void setUnits(QString units);
	QString units() const {
		return units_;
	}

	QJsonArray createJSONDescriptor();
	bool init(QJsonArray& arr);

protected:
	virtual void keyPressEvent(QKeyEvent* ev) override;
	virtual void paintEvent(QPaintEvent* ev) override;

private:
	void arrangeCharts();

private:
	PlotManager& plot_mgr_;
	QGridLayout* layout_;
	QtCharts::QChartView* selected_;
	std::vector<SingleChart *> charts_;
	QString units_;
};
