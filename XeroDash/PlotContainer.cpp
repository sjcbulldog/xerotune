#include "PlotContainer.h"

PlotContainer::PlotContainer(PlotManager& mgr, QWidget *parent) : QWidget(parent), plot_mgr_(mgr)
{
	selected_ = nullptr;
	layout_ = new QGridLayout(this);

	setLayout(layout_);
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
}

PlotContainer::~PlotContainer()
{
}

void PlotContainer::keyPressEvent(QKeyEvent *ev)
{
	if (ev->key() == Qt::Key::Key_Insert)
	{
		if (charts_.size() == 4)
			return;

		SingleChart* ch = new SingleChart(plot_mgr_, this);
		charts_.push_back(ch);
		layout_->addWidget(ch, 0, 0);
		arrangeCharts();
	}
	else if (ev->key() == Qt::Key::Key_Delete)
	{
	}
}

void PlotContainer::paintEvent(QPaintEvent* ev)
{
	QPainter paint(this);
	QRectF r(0, 0, width(), height());
	QBrush b(QColor(0xc0, 0xc0, 0xc0, 0xff));
	paint.setBrush(b);
	paint.drawRect(r);
}

void PlotContainer::arrangeCharts()
{
	for (auto w : charts_)
		layout_->removeWidget(w);

	if (charts_.size() == 1)
	{
		layout_->addWidget(charts_[0], 0, 0);
	}
	else if (charts_.size() == 2)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
	}
	if (charts_.size() == 3)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
	}
	else if (charts_.size() == 4)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
	}
}