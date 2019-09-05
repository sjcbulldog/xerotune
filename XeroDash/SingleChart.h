#pragma once

#include "Callout.h"
#include "PlotDescriptor.h"
#include "PlotManager.h"
#include "TabEditName.h"
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QString>
#include <QJsonObject>
#include <string>
#include <map>

class SingleChart : public QtCharts::QChartView
{
	Q_OBJECT

public:
	SingleChart(QString units, PlotManager& mgr, QWidget *parent = Q_NULLPTR);
	~SingleChart();

	void setUnits(QString units);
	QString units() const {
		return units_;
	}

	void setTitle(QString title);

	QString title() const {
		return title_;
	}

	QJsonObject createJSONDescriptor();

	bool init(QJsonObject obj);

	void editTitle();

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dragMoveEvent(QDragMoveEvent* event) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
	virtual void dropEvent(QDropEvent* ev) override;
	virtual void keyPressEvent(QKeyEvent* ev) override;
	virtual void focusInEvent(QFocusEvent* ev) override;

private:
	void insertNode(QString node);
	void insertNode(QString ds, QString node);
	QtCharts::QValueAxis* findAxis(QString axisname);
	QtCharts::QLineSeries* findSeries(QString node);
	QString nodeToAxis(QString node);

	void setMinMax(const std::string& name, double minv, double maxv);
	void getMinMax(const std::string& name, double &minv, double &maxv);

	void dataComplete();
	void activeChanged();

	void createTimeAxis();
	void createLegend();

	void seriesHover(QtCharts::QLineSeries* series, const QPointF& point, bool state);
	void seriesClick(QtCharts::QLineSeries* series, const QPointF& point);

	void datasetActive();

	void editorDone();
	void editorAborted();

private:
	PlotManager& plot_mgr_;
	std::shared_ptr<PlotDescriptor> desc_;
	QtCharts::QValueAxis* time_;
	QtCharts::QAbstractSeries* first_;
	QtCharts::QLegend* legend_;
	double tminv_, tmaxv_;
	std::list<QString> node_names_;
	size_t index_;
	std::map<std::string, std::pair<double, double>> min_max_;

	int total_scroll_x_;
	int total_scroll_y_;

	Callout* callout_;
	std::list<Callout*> callouts_;

	QString units_;
	QString title_;

	int left_axis_count_;
	int right_axis_count_;

	std::list<std::pair<QString, QString>> pending_;
	QMetaObject::Connection connection_;

	TabEditName* editor_;
};
