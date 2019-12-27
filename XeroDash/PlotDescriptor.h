#pragma once

#include <QObject>
#include <QString>
#include <mutex>
#include <string>
#include <vector>

class PlotDescriptor : public QObject
{
	Q_OBJECT;

public:
	PlotDescriptor(const std::string &name);
	virtual ~PlotDescriptor();

	bool hasData(size_t index) {
		if (index >= valid_.size())
			return false;

		return valid_[index];
	}

	const std::string &name() const {
		return name_;
	}

	void clearColumns() {
		std::lock_guard<std::mutex> guard(lock_);
		columns_.clear();
	}

	void addColumn(const std::string& col) {
		std::lock_guard<std::mutex> guard(lock_);
		columns_.push_back(col);
	}
	
	std::vector<std::string> columns() {
		std::vector<std::string> copy = columns_;
		return copy;
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
	std::string name_ ;
	std::vector<std::string> columns_;
	bool inited_;
	bool active_;
	std::mutex lock_;
	std::vector<bool> valid_;
	std::vector<std::vector<double>> data_;
};

