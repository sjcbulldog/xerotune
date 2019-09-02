#include "SingleChart.h"
#include <QMimeData>
#include <QListWidget>
#include <QDebug>

using namespace QtCharts;

SingleChart::SingleChart(PlotManager &mgr, QWidget *parent) : QChartView(parent), plot_mgr_(mgr)
{
	chart()->setDropShadowEnabled(true);
	setAcceptDrops(true);

	time_ = nullptr;
}

SingleChart::~SingleChart()
{
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
		}
	}
	setBackgroundRole(QPalette::Background);
}

QValueAxis* SingleChart::findAxis(QString node)
{
	for (auto axis : chart()->axes())
	{
		if (axis->titleText() == node)
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
	return "Axis";
}

void SingleChart::insertNode(QString node)
{
	std::shared_ptr<PlotDescriptor> desc = plot_mgr_.current();

	if (desc_ == nullptr)
	{
		desc_ = desc;
	}
	else
	{
		if (desc != desc_)
			return;
	}

	if (!desc_->hasColumns() || !desc_->hasData())
		return;

	size_t colidx = desc_->getColumnIndexFromName(node.toStdString());
	if (colidx == std::numeric_limits<size_t>::max())
		return;

	if (time_ == nullptr)
	{
		time_ = new QValueAxis();
		time_->setLabelsVisible(true);
		time_->setTickCount(10);
		time_->setTitleText("time (s)");
		time_->setTitleVisible(true);
		chart()->addAxis(time_, Qt::AlignBottom);
	}

	QString axisname = nodeToAxis(node);
	QValueAxis* axis = findAxis(axisname);
	if (axis == nullptr)
	{
		axis = new QValueAxis();
		axis->setLabelsVisible(true);
		axis->setTickCount(10);
		axis->setTitleText("Axis");
		axis->setTitleVisible(true);
		chart()->addAxis(axis, Qt::AlignLeft);
	}

	QLineSeries* series = findSeries(node);
	if (series == nullptr)
	{
		double minv = std::numeric_limits<double>::max();
		double maxv = std::numeric_limits<double>::min();

		series = new QLineSeries();

		for (size_t row = 0; row < desc_->dataCount(); row++)
		{
			double d = desc_->data(row, colidx);
			double t = desc_->data(row, 0);

			if (d > maxv)
				maxv = d;

			if (d < minv)
				minv = d;

			series->append(t, d);
			qDebug() << "Point " << t << " " << d;
		}

		time_->setRange(0, 10);
		axis->setRange(0, 100);

		series->attachAxis(time_);
		series->attachAxis(axis);
		chart()->addSeries(series);

		axis->applyNiceNumbers();
		time_->applyNiceNumbers();
	}
}
