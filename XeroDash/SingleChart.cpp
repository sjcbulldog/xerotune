#include "SingleChart.h"
#include "PlotContainer.h"
#include <QMimeData>
#include <QListWidget>
#include <QDebug>

using namespace QtCharts;

SingleChart::SingleChart(QString units, PlotManager &mgr, QWidget *parent) : QChartView(parent), plot_mgr_(mgr)
{
	chart()->setDropShadowEnabled(true);
	setAcceptDrops(true);

	time_ = nullptr;
	legend_ = nullptr;

	tminv_ = std::numeric_limits<double>::max();
	tmaxv_ = std::numeric_limits<double>::min();

	index_ = 0;

	total_scroll_x_ = 0;
	total_scroll_y_ = 0;

	setRubberBand(QChartView::RectangleRubberBand);

	units_ = units;
	callout_ = nullptr;
	first_ = nullptr;

	left_axis_count_ = 0;
	right_axis_count_ = 0;
}

SingleChart::~SingleChart()
{
}

void SingleChart::setUnits(QString units)
{
	QString oldunits = units_;

	units_ = units;
	for (auto axis : chart()->axes())
	{
		QString newname = axis->titleText().replace(oldunits, units_);
		axis->setTitleText(newname);
	}
}

void SingleChart::focusInEvent(QFocusEvent* ev)
{
	PlotContainer* c = dynamic_cast<PlotContainer*>(parent());
	if (c != nullptr)
		c->childFocused(this);
}

void SingleChart::keyPressEvent(QKeyEvent* ev)
{
	switch (ev->key()) {
	case Qt::Key_Plus:
		chart()->zoomIn();
		break;
	case Qt::Key_Minus:
		chart()->zoomOut();
		break;
	case Qt::Key_Left:
		chart()->scroll(-10, 0);
		total_scroll_x_ -= 10;
		break;
	case Qt::Key_Right:
		chart()->scroll(10, 0);
		total_scroll_x_ += 10;
		break;
	case Qt::Key_Up:
		chart()->scroll(0, 10);
		total_scroll_y_ += 10;
		break;
	case Qt::Key_Down:
		chart()->scroll(0, -10);
		total_scroll_y_ -= 10;
		break;
	case Qt::Key_Home:
		chart()->scroll(-total_scroll_x_, -total_scroll_y_);
		chart()->zoomReset();
		total_scroll_x_ = 0;
		total_scroll_y_ = 0;
		break;
	default:
		QGraphicsView::keyPressEvent(ev);
		break;
	}
}

void SingleChart::setMinMax(const std::string& name, double minv, double maxv)
{
	auto it = min_max_.find(name);
	if (it == min_max_.end())
		min_max_[name] = std::make_pair(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());

	auto pair = min_max_[name];
	if (minv < pair.first)
		pair.first = minv;

	if (maxv > pair.second)
		pair.second = maxv;

	min_max_[name] = pair;
}

void SingleChart::getMinMax(const std::string& name, double &minv, double &maxv)
{
	auto it = min_max_.find(name);
	if (it == min_max_.end())
	{
		minv = 0.0;
		maxv = 10.0;
	}
	else
	{
		minv = min_max_[name].first;
		maxv = min_max_[name].second;
	}

	if (std::fabs(minv - maxv) < 0.1)
	{
		if (std::fabs(minv) < 0.01)
		{
			minv = -1.0;
			maxv = 1.0;
		}
		else if (minv < 0)
		{
			maxv = 0.0;
			minv *= 2.0;
		}
		else
		{
			minv = 0.0;
			maxv *= 2.0;
		}
	}
}

void SingleChart::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void SingleChart::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void SingleChart::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
}

void SingleChart::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* listMimeType = "application/x-qabstractitemmodeldatalist";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(listMimeType))
	{
		QByteArray encoded = data->data(listMimeType);
		QDataStream stream(&encoded, QIODevice::ReadOnly);

		while (!stream.atEnd())
		{
			int row, col;
			QMap<int, QVariant> roleDataMap;
			stream >> row >> col >> roleDataMap;

			QVariant v = roleDataMap[Qt::ItemDataRole::DisplayRole];
			insertNode(v.toString());
			node_names_.push_back(v.toString());
		}
	}
	setBackgroundRole(QPalette::Background);
}

QValueAxis* SingleChart::findAxis(QString node)
{
	for (auto axis : chart()->axes())
	{
		QString axisname = axis->titleText();
		if (axisname == node)
			return dynamic_cast<QValueAxis *>(axis);
	}

	return nullptr;
}

QLineSeries* SingleChart::findSeries(QString node)
{
	for (auto series : chart()->series())
	{
		if (series->name() == node)
			return dynamic_cast<QLineSeries*>(series);
	}
	return nullptr;
}

QString SingleChart::nodeToAxis(QString node)
{
	if (node.contains("dist"))
		return "Distance (" + units_ + ")";
	else if (node.contains("velocity"))
		return "Velocity (" + units_ + "/s)";
	else if (node.contains("accel"))
		return "Accel (" + units_ + "/s^2)";
	else if (node.contains("jerk"))
		return "Jerk (" + units_ + " / s ^ 3)";
	else if (node.contains("tick"))
		return "Ticks (count)";

	return node;
}

void SingleChart::createTimeAxis()
{
	time_ = new QValueAxis();
	time_->setLabelsVisible(true);
	time_->setTickCount(10);
	time_->setTitleText("time (s)");
	time_->setTitleVisible(true);
	chart()->addAxis(time_, Qt::AlignBottom);
}

void SingleChart::createLegend()
{
	QLegend* legend = chart()->legend();
	legend->setVisible(true);
	legend->setAlignment(Qt::AlignBottom);
	legend->setMarkerShape(QLegend::MarkerShape::MarkerShapeCircle);
	QFont font = legend->font();
	font.setPointSize(8);
	font.setBold(true);
	legend->setFont(font);
}

void SingleChart::insertNode(QString node)
{
	std::shared_ptr<PlotDescriptor> desc = plot_mgr_.current();

	if (desc_ == nullptr)
	{
		desc_ = desc;
		(void)connect(desc_.get(), &PlotDescriptor::dataAdded, this, &SingleChart::dataAdded);
		(void)connect(desc_.get(), &PlotDescriptor::activeChanged, this, &SingleChart::activeChanged);
	}
	else
	{
		if (desc != desc_)
			return;
	}

	if (!desc_->hasColumns() || !desc_->hasData())
		return;

	size_t colidx = desc_->getColumnIndexFromName(node.toStdString());
	if (colidx == std::numeric_limits<size_t>::max() || colidx == 0)
		return;

	if (time_ == nullptr)
		createTimeAxis();

	if (legend_ == nullptr)
		createLegend();

	QString axisname = nodeToAxis(node);
	QValueAxis* axis = findAxis(axisname);
	if (axis == nullptr)
	{
		axis = new QValueAxis();

		axis->setLabelsVisible(true);
		axis->setTickCount(10);
		axis->setTitleText(axisname);
		axis->setTitleVisible(true);

		if (left_axis_count_ <= right_axis_count_) {
			chart()->addAxis(axis, Qt::AlignLeft);
			left_axis_count_++;
		}
		else
		{
			chart()->addAxis(axis, Qt::AlignRight);
			right_axis_count_++;
		}
	}

	double maxv = std::numeric_limits<double>::min();
	double minv = std::numeric_limits<double>::max();

	QLineSeries* series = findSeries(node);
	if (series == nullptr)
	{
		series = new QLineSeries();
		if (first_ == nullptr)
			first_ = series;

		(void)connect(series, &QLineSeries::hovered, this, [series, this](const QPointF& pt, bool state) { this->seriesHover(series, pt, state); });
		(void)connect(series, &QLineSeries::clicked, this, [series, this](const QPointF& pt) { this->seriesClick(series, pt); });


		series->setName(node);

		for (size_t row = 0; row < index_; row++)
		{
			double d = desc_->data(row, colidx);
			double t = desc_->data(row, 0);

			if (d > maxv)
				maxv = d;

			if (d < minv)
				minv = d;

			if (t > tmaxv_)
				tmaxv_ = t;

			if (t < tminv_)
				tminv_ = t;

			series->append(t, d);
		}

		setMinMax(axisname.toStdString(), minv, maxv);

		chart()->addSeries(series);

		time_->setRange(tminv_, tmaxv_);

		getMinMax(axisname.toStdString(), minv, maxv);
		axis->setRange(minv, maxv);

		series->attachAxis(time_);
		series->attachAxis(axis);

		axis->applyNiceNumbers();
		time_->applyNiceNumbers();
	}

	dataAdded();
}

void SingleChart::activeChanged()
{
	if (desc_->active())
	{
		index_ = 0;

		for (QAbstractSeries* series : chart()->series())
		{
			QLineSeries* line = dynamic_cast<QLineSeries*>(series);
			line->clear();
		}
	}
	else
	{
		//
		// Transitioned from active to inactive, data is all here
		//
		for (auto axis : chart()->axes())
		{
			QValueAxis* v = dynamic_cast<QValueAxis*>(axis);
			v->applyNiceNumbers();
		}
	}
}

void SingleChart::dataAdded()
{
	size_t last = desc_->dataCount();
	if (index_ == last)
		return;

	for (QAbstractSeries* series : chart()->series())
	{
		double maxv = std::numeric_limits<double>::min();
		double minv = std::numeric_limits<double>::max();

		QLineSeries* line = dynamic_cast<QLineSeries*>(series);
		if (line != nullptr)
		{
			size_t colidx = desc_->getColumnIndexFromName(line->name().toStdString());
			if (colidx < desc_->columns().size())
			{
				for (size_t i = index_; i < last; i++)
				{
					double t = desc_->data(i, 0);
					double d = desc_->data(i, colidx);
					line->append(t, d);

					if (d > maxv)
						maxv = d;

					if (d < minv)
						minv = d;

					if (t > tmaxv_)
						tmaxv_ = t;

					if (t < tminv_)
						tminv_ = t;
				}
			}

			QString axisname = nodeToAxis(line->name());

			time_->setRange(tminv_, tmaxv_);
			setMinMax(axisname.toStdString(), minv, maxv);
			getMinMax(axisname.toStdString(), minv, maxv);

			QValueAxis* axis = findAxis(axisname);
				axis->setRange(minv, maxv);
		}
	}

	index_ = last;
}

void SingleChart::seriesHover(QLineSeries* ser, const QPointF& pt, bool state)
{
	QPointF npt = chart()->mapToPosition(pt, ser);
	npt = chart()->mapToValue(npt, first_);

	(void)ser;
	if (callout_ == nullptr)
		callout_ = new Callout(chart());

	if (state)
	{
		QString text;

		text += "Time: " + QString::number(pt.x(), 'f', 2) + "\n";
		text += ser->name() + ": " + QString::number(pt.y(), 'f', 1);

		callout_->setText(text);
		callout_->setAnchor(npt);
		callout_->setZValue(11);
		callout_->updateGeometry();
		callout_->show();
	}
	else
		callout_->hide();
}


void SingleChart::seriesClick(QLineSeries* ser, const QPointF& pt)
{
	(void)ser;
	(void)pt;

	callouts_.push_back(callout_);
	callout_ = nullptr;
}