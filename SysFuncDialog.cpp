#include "SysFuncDialog.h"
#include "ui_SysFuncDialog.h"
#include "HaltDialog.h"
#include "GPIOAdapter.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
SysFuncDialog::SysFuncDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SysFuncDialog)
{
	ui->setupUi(this);
	timer=new QTimer(this);
	singleShot=new QTimer(this);
	timer->setInterval(1000);
	singleShot->setInterval(5000);
	count=21;

	connect(singleShot,SIGNAL(timeout()),singleShot,SLOT(stop()));
	connect(singleShot,SIGNAL(timeout()),timer,SLOT(start()));
	connect(timer,SIGNAL(timeout()),this,SLOT(flushCounter()));

	connect(ui->close,SIGNAL(clicked()),this,SLOT(close()));

	connect(ui->halt,SIGNAL(clicked()),this,SLOT(halt()));
	connect(ui->reboot,SIGNAL(clicked()),this,SLOT(reboot()));

	connect(ui->halt,SIGNAL(clicked()),this,SLOT(hide()));
	connect(ui->reboot,SIGNAL(clicked()),this,SLOT(hide()));
	connect(ui->exit,SIGNAL(clicked()),qApp,SLOT(quit()));

	connect(ui->backlightSlider,SIGNAL(valueChanged(int)),GPIOAdapter::getInstance(),SLOT(setLCDBrightness(int)));
	connect(ui->backlightSlider,SIGNAL(valueChanged(int)),this,SLOT(resetTimer()));
	connect(ui->backlightSlider,SIGNAL(sliderPressed()),this,SLOT(resetTimer()));

	connect(GPIOAdapter::getInstance(),SIGNAL(LCDBrightnessReturned(int)),ui->backlightSlider,SLOT(setValue(int)));
	connect(GPIOAdapter::getInstance(),SIGNAL(LCDBrightnessReturned(int)),this,SLOT(brightnessLoaded()));

	GPIOAdapter::getInstance()->getLCDBrightness();

	ui->backlightSlider->installEventFilter(this);

	singleShot->start();
}

SysFuncDialog::~SysFuncDialog(){
	delete ui;
	delete timer;
}
void SysFuncDialog::halt(){
	HaltDialog::getInstance()->halt(10);
}

void SysFuncDialog::reboot(){
	HaltDialog::getInstance()->reboot(10);
}
void SysFuncDialog::flushCounter(){
	count--;
	ui->count->setText(QString("%1秒后自动关闭窗口").arg(QString::number(count)));
	if(count<=0){
		timer->stop();
		this->close();
	}
}
void SysFuncDialog::resetTimer(){
	if(singleShot->isActive()){
		singleShot->stop();
	}else if(timer->isActive()){
		timer->stop();
		count=21;
		ui->count->setText(QString());
	}
	singleShot->start();
}

bool SysFuncDialog::eventFilter(QObject *obj,QEvent *event){
	if(obj==ui->backlightSlider){
		if (event->type()==QEvent::MouseButtonPress){
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			if (mouseEvent->button() == Qt::LeftButton){
				int dur = ui->backlightSlider->maximum() - ui->backlightSlider->minimum();
				int pos = ui->backlightSlider->minimum() + dur * ((double)mouseEvent->x() / ui->backlightSlider->width());
				if(pos != ui->backlightSlider->sliderPosition()){
					ui->backlightSlider->setValue(pos);
				}
			}
		}
	}
	return QObject::eventFilter(obj,event);
}
void SysFuncDialog::brightnessLoaded(){
	ui->backlightSlider->setEnabled(true);
}
