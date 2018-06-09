#include "SysFuncDialog.h"
#include "ui_SysFuncDialog.h"
#include "HaltDialog.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QFile>
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

    connect(ui->halt,SIGNAL(clicked(bool)),this,SLOT(deleteLater()));
    connect(ui->reboot,SIGNAL(clicked(bool)),this,SLOT(deleteLater()));
    connect(ui->close,SIGNAL(clicked(bool)),this,SLOT(deleteLater()));

	connect(ui->exit,SIGNAL(clicked()),qApp,SLOT(quit()));

    int backlight=this->backlight();
    ui->backlightSlider->setEnabled(backlight>0);
    ui->backlightSlider->setValue(backlight);

    connect(ui->backlightSlider,SIGNAL(valueChanged(int)),this,SLOT(resetTimer()));
	connect(ui->backlightSlider,SIGNAL(sliderPressed()),this,SLOT(resetTimer()));
    connect(ui->backlightSlider,SIGNAL(valueChanged(int)),this,SLOT(setBacklight(int)));

	ui->backlightSlider->installEventFilter(this);

    singleShot->start();
}



SysFuncDialog::~SysFuncDialog(){
	delete ui;
    delete timer;
}

int SysFuncDialog::backlight(){
    QFile blFile("/sys/class/backlight/rpi_backlight/brightness");
    if(!blFile.open(QIODevice::ReadOnly)){
        qDebug()<<"获取屏幕亮度失败:"<<blFile.errorString();
        return -1;
    }
    int backlight=blFile.readLine().replace("\n","").toInt();
    blFile.close();
    return backlight;
}

void SysFuncDialog::setBacklight(int rate){
    QFile blFile("/sys/class/backlight/rpi_backlight/brightness");
    if(!blFile.open(QIODevice::WriteOnly)){
        qDebug()<<"无法设置亮度(文件错误):"<<blFile.errorString();
        return;
    }

    blFile.write(QString::number(rate).toLocal8Bit());
    blFile.close();
}

void SysFuncDialog::halt(){
    HaltDialog::instance()->halt(10,true);
}

void SysFuncDialog::reboot(){
    HaltDialog::instance()->reboot(10);
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
