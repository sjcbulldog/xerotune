#pragma once

#include "PlotDescriptor.h"
#include <memory>

class PlotNode
{
public:
	PlotNode(std::shared_ptr<PlotDescriptor> desc, size_t col) {
		desc_ = desc;
		col_ = col;
	}

	virtual ~PlotNode() {
	}

	std::shared_ptr<PlotDescriptor> plotDesc() {
		return desc_;
	}

	size_t column() {
		return col_;
	}

private:
	std::shared_ptr<PlotDescriptor> desc_;
	size_t col_;
};

