#include "HaltDialog.h"
#include "ui_HaltDialog.h"
#include "GPIOAdapter.h"
#include <QDebug>
#include <QProcess>
#include <QTimer>
HaltDialog* HaltDialog::instance=NULL;

HaltDialog::HaltDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::HaltDialog)
{
	ui->setupUi(this);
	map.insert("halt","关机");
	map.insert("reboot","重启");
	timer=new QTimer(this);
	timer->setInterval(1000);
	beep=false;
	connect(timer,SIGNAL(timeout()),this,SLOT(flushTitle()));
	connect(ui->cancel,SIGNAL(clicked()),this,SLOT(close()));
	connect(ui->cancel,SIGNAL(clicked()),timer,SLOT(stop()));
}

HaltDialog::~HaltDialog(){
	delete ui;
	delete timer;
}
HaltDialog* HaltDialog::getInstance(){
	if(HaltDialog::instance==NULL)
		HaltDialog::instance=new HaltDialog();
	return HaltDialog::instance;
}
void HaltDialog::doAction(){
	if(QProcess::startDetached("sudo "+action))
		qApp->quit();
}

void HaltDialog::flushTitle(){
	delay--;
	ui->title->setText(QString("系统将于%1秒后%2!").arg(QString::number(delay),map.value(action)));
	if(delay<=0){
		timer->stop();
		if(beep){
			ui->cancel->setEnabled(false);
			ui->cancel->setText(QString("即将%1").arg(map.value(action)));
			GPIOAdapter::beep(3,100);
			QTimer::singleShot(400,this,SLOT(doAction()));
		}else{
			doAction();
		}
	}
}
void HaltDialog::halt(int delay, bool beep){
	this->action="halt";
	this->delay=delay;
	this->beep=beep;
	if(this->delay>1){
		this->flushTitle();
		timer->start();
		this->show();
	}else{
		if(beep){
			GPIOAdapter::beep(3,100);
			QTimer::singleShot(400,this,SLOT(doAction()));
		}else{
			doAction();
		}
	}
}

void HaltDialog::reboot(int delay, bool beep){
	this->action="reboot";
	this->delay=delay+1;
	this->beep=beep;
	if(this->delay>1){
		this->flushTitle();
		timer->start();
		this->show();
	}else{
		if(beep){
			GPIOAdapter::beep(3,100);
			QTimer::singleShot(400,this,SLOT(doAction()));
		}else{
			doAction();
		}
	}
}
