#pragma once

#include "Callout.h"
#include "PlotDescriptor.h"
#include "PlotManager.h"
#include "TabEditName.h"
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <string>
#include <map>

class SingleChart : public QtCharts::QChartView
{
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

	void highlight(bool high);

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dragMoveEvent(QDragMoveEvent* event) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
	virtual void dropEvent(QDropEvent* ev) override;
	virtual void keyPressEvent(QKeyEvent* ev) override;
	virtual void mouseMoveEvent(QMouseEvent* ev) override;
	virtual void focusInEvent(QFocusEvent* ev) override;

private:
	void insertNode(QString node);
	void insertNode(QString ds, QString node);
	QtCharts::QValueAxis* findAxis(QString axisname);
	QtCharts::QLineSeries* findSeries(QString node);
	QString nodeToAxis(QString node);

	void setMinMax(const std::string& name, double minv, double maxv);
	void getMinMax(const std::string& name, double &minv, double &maxv);

	void dataAdded();
	void dataReset();

	void createTimeAxis();
	void createLegend();

	void seriesHover(QtCharts::QLineSeries* series, const QPointF& point, bool state);
	void seriesClick(QtCharts::QLineSeries* series, const QPointF& point);

	void newPlotAdded();

	void editorDone();
	void editorAborted();

	QStringList computeAverages();
	QGraphicsLineItem* createLine(double stime, QPen p);
	void displayAverages();
	void clearAnnotations();
	int findClosest(QVector<QPointF>& pts, double v);

private:
	PlotManager& plot_mgr_;
	std::shared_ptr<PlotDescriptor> desc_;
	QtCharts::QValueAxis* time_;
	QtCharts::QAbstractSeries* first_;
	QtCharts::QLegend* legend_;
	double tminv_, tmaxv_;
	std::list<QString> node_names_;
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

	QPointF mouse_;

	bool first_point_;
	bool first_valid_;
	bool second_valid_;
	double first_value_;
	double second_value_;
	QGraphicsLineItem *first_line_;
	QGraphicsLineItem* second_line_;
	QGraphicsRectItem* rect_item_;
	std::list<QGraphicsSimpleTextItem*> text_items_;
};
