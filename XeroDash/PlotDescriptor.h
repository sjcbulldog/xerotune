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

	void addData(size_t index, const std::vector<double>& data);
	void resize(size_t index);

	bool isDataValid(size_t row) {
		return valid_[row];
	}

	std::vector<std::vector<double>> data() {
		return data_;
	}

	size_t dataSize() const {
		return data_.size();
	}

	void signalDataAdded() {
		emitDataAdded();
	}

	void signalDataReset() {
		emitDataReset();
	}


signals:
	void dataAdded();
	void dataReset();

private:
	void emitDataAdded();
	void emitDataReset();

private:
	double percent_;
	QListWidgetItem* item_;
	std::vector<std::string> columns_;
	bool inited_;
	bool active_;
	std::mutex lock_;
	std::vector<bool> valid_;
	std::vector<std::vector<double>> data_;
};

