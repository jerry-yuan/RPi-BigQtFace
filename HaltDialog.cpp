#include "HaltDialog.h"
#include "ui_HaltDialog.h"
#include "GPIOAdapter.h"
#include "EventServer.h"
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QtMath>
HaltDialog* HaltDialog::m_instance=NULL;

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
    EventServer::instance()->addMethod(this,"triggerHalt");
    EventServer::instance()->addMethod(this,"triggerReboot");
}

HaltDialog::~HaltDialog(){
	delete ui;
	delete timer;
}
HaltDialog* HaltDialog::instance(){
    static QMutex insMutex;
    if(!HaltDialog::m_instance){
        QMutexLocker locker(&insMutex);
        if(!HaltDialog::m_instance)
            HaltDialog::m_instance=new HaltDialog();
    }
    return HaltDialog::m_instance;
}
void HaltDialog::doAction(){
	if(QProcess::startDetached("sudo "+action))
		qApp->quit();
}

void HaltDialog::triggerHalt(QVariantHash params){
    int delay=params.value("delay").toInt();
    bool beep=params.value("beep").toBool();
    if(delay<1) return;
    this->halt(delay,beep);
}

void HaltDialog::triggerReboot(QVariantHash params){
    int delay=params.value("delay").toInt();
    bool beep=params.value("beep").toBool();
    if(delay<1) return;
    this->reboot(delay,beep);
}

void HaltDialog::flushTitle(){
    this->delay=qMax<quint64>(QDateTime::currentDateTime().secsTo(this->triggerTime),0);
    ui->title->setText(QString("系统将于%1秒后%2!").arg(delay).arg(map.value(action)));
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
    this->triggerTime=QDateTime::currentDateTime().addSecs(delay);
	this->beep=beep;
    if(beep)
        GPIOAdapter::beep(2,100);
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
    this->triggerTime=QDateTime::currentDateTime().addSecs(delay);
    this->beep=beep;
    if(beep)
        GPIOAdapter::beep(2,100);
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
