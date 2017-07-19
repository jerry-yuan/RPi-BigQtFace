#include "Weather24hDisplayItem.h"
#include "ui_Weather24hDisplayItem.h"

Weather24hDisplayItem::Weather24hDisplayItem(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Weather24hDisplayItem)
{
	ui->setupUi(this);
}

Weather24hDisplayItem::~Weather24hDisplayItem()
{
	delete ui;
}
