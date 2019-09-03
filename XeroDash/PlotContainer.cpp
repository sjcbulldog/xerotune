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

void PlotContainer::childFocused(SingleChart* ch)
{
	auto it = std::find(charts_.begin(), charts_.end(), ch);
	if (it != charts_.end())
		selected_ = ch;
}

void PlotContainer::keyPressEvent(QKeyEvent *ev)
{
	if (ev->key() == Qt::Key::Key_Insert)
	{
		if (charts_.size() == 9)
			return;

		SingleChart* ch = new SingleChart(plot_mgr_, this);
		charts_.push_back(ch);
		layout_->addWidget(ch, 0, 0);
		arrangeCharts();
	}
	else if (ev->key() == Qt::Key::Key_Delete)
	{
		auto it = std::find(charts_.begin(), charts_.end(), selected_);
		if (it != charts_.end())
		{
			charts_.erase(it);
			delete selected_;
			selected_ = nullptr;
			if (charts_.size() > 0)
				selected_ = charts_.front();

			arrangeCharts();
		}
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
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 0);
		layout_->setRowStretch(2, 0);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 0);
		layout_->setColumnStretch(2, 0);
	}
	else if (charts_.size() == 2)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 1, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 0);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 0);
		layout_->setColumnStretch(2, 0);
	}
	if (charts_.size() == 3)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 0);
	}
	else if (charts_.size() == 4)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 0);
	}
	else if (charts_.size() == 5)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->addWidget(charts_[4], 2, 0, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 6)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 1, 0);
		layout_->addWidget(charts_[3], 1, 1);
		layout_->addWidget(charts_[4], 2, 0);
		layout_->addWidget(charts_[5], 2, 1);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 0);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 7)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 0, 2);
		layout_->addWidget(charts_[3], 1, 0);
		layout_->addWidget(charts_[4], 1, 1);
		layout_->addWidget(charts_[5], 1, 2);
		layout_->addWidget(charts_[6], 2, 0, 1, 3);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 8)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 0, 2);
		layout_->addWidget(charts_[3], 1, 0);
		layout_->addWidget(charts_[4], 1, 1);
		layout_->addWidget(charts_[5], 1, 2);
		layout_->addWidget(charts_[6], 2, 0);
		layout_->addWidget(charts_[7], 2, 1, 1, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
	else if (charts_.size() == 9)
	{
		layout_->addWidget(charts_[0], 0, 0);
		layout_->addWidget(charts_[1], 0, 1);
		layout_->addWidget(charts_[2], 0, 2);
		layout_->addWidget(charts_[3], 1, 0);
		layout_->addWidget(charts_[4], 1, 1);
		layout_->addWidget(charts_[5], 1, 2);
		layout_->addWidget(charts_[6], 2, 0);
		layout_->addWidget(charts_[7], 2, 1);
		layout_->addWidget(charts_[8], 2, 2);
		layout_->setColumnStretch(0, 100);
		layout_->setColumnStretch(1, 100);
		layout_->setColumnStretch(2, 100);
		layout_->setRowStretch(0, 100);
		layout_->setRowStretch(1, 100);
		layout_->setRowStretch(2, 100);
	}
}