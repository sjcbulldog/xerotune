#pragma once

#include <QDialog>
#include "ui_KeyHints.h"

class KeyHints : public QDialog
{
	Q_OBJECT

public:
	KeyHints(QWidget *parent = Q_NULLPTR);
	~KeyHints();

private:
	Ui::KeyHints ui;
};
