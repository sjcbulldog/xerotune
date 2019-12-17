#include "KeyHints.h"

KeyHints::KeyHints(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.buttons_, &QDialogButtonBox::accepted, this, &KeyHints::accept);
	connect(ui.buttons_, &QDialogButtonBox::rejected, this, &KeyHints::reject);

	ui.text_->setReadOnly(true);
	ui.text_->append("Keyboard Keys\r\n\r\n");
	ui.text_->append("Insert Key - Insert new plot in current window\r\n");
	ui.text_->append("Delete key - Delete current plot in current window\r\n");
	ui.text_->append("Plus key - Zoom in current plot\r\n");
	ui.text_->append("Minus key - Zoom out current plot\r\n");
	ui.text_->append("Left key - scroll current plot left\r\n");
	ui.text_->append("Right key - scroll current plot right\r\n");
	ui.text_->append("Up key - scroll current plot up\r\n");
	ui.text_->append("Down key - scroll current plot down\r\n");
	ui.text_->append("Home key - restore plot to it initial scroll/zoom\r\n");
	ui.text_->append("Space key - set first or second point for average calculation\r\n");
}

KeyHints::~KeyHints()
{
}
