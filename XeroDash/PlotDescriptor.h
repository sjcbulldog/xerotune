#pragma once

#include <QObject>
#include <QString>
#include <QListWidgetItem>
#include <mutex>
#include <string>
#include <vector>

class PlotDescriptor : public QObject
{
	Q_OBJECT;

public:
	PlotDescriptor(QListWidgetItem* item);
	virtual ~PlotDescriptor();

	bool isConsolidated() const {
		return consolidated_;
	}

	bool hasData(size_t index) {
		if (index >= valid_.size())
			return false;

		return valid_[index];
	}

	double percentCaptured() {
		return percent_;
	}

	QString name() const {
		return item_->text();
	}

	QListWidgetItem* item() {
		return item_;
	}

	bool inited() const {
		return inited_;
	}

	void setInited(bool b) {
		inited_ = b;
		emitInitedChanged();
	}

	bool active() const {
		return active_;
	}

	void setActive(bool b);

	void enable();

	void clearColumns() {
		columns_.clear();
	}

	void addColumn(const std::string& col) {
		columns_.push_back(col);
	}
	
	const std::vector<std::string>& columns() {
		return columns_;
	}

	size_t getColumnIndexFromName(const std::string& name) const;

	bool hasColumns() const {
		return columns_.size() > 0;
	}

	bool hasData() const {
		return data_.size() > 0;
	}

	void addData(int index, const std::vector<double>& data);

	size_t dataCount() const {
		return data_.size();
	}

	double data(size_t row, size_t col) {
		return (data_[row])[col];
	}

	void consolidate();

signals:
	void activeChanged();
	void initedChanged();
	void dataCompleted();

private:
	void emitActiveChanged();
	void emitInitedChanged();
	void emitDataCompleted();

private:
	double percent_;
	QListWidgetItem* item_;
	std::vector<std::string> columns_;
	bool inited_;
	bool active_;
	std::mutex lock_;
	std::vector<bool> valid_;
	std::vector<std::vector<double>> data_;
	bool consolidated_;
};

