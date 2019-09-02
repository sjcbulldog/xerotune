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

	void setActive(bool b) {

		if (!active_ && b)
		{
			//
			// We are transitioning from not active to active, clear data and reset index counter
			//
			data_.clear();
			index_ = 0;
			total_lost_ = 0;
		}

		active_ = b;
		emitActiveChanged();
	}

	bool dataDirty() const {
		return data_dirty_;
	}

	void setDataDirty(bool b) {
		lock_.lock();
		data_dirty_ = b;
		lock_.unlock();

		if (b)
			emitDataAdded();
	}

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

signals:
	void activeChanged();
	void initedChanged();
	void dataAdded();

private:
	void emitActiveChanged();
	void emitInitedChanged();
	void emitDataAdded();

private:
	QListWidgetItem* item_;
	std::vector<std::string> columns_;
	bool inited_;
	bool active_;
	std::mutex lock_;
	bool data_dirty_;
	int index_;
	int total_lost_;
	std::vector<std::vector<double>> data_;
};

