#include "Clock.h"
#include <QTimer>
#include <QLCDNumber>
#include <QDateTime>
#include "GPIOAdapter.h"
Clock::Clock(QWidget *parent):QLCDNumber(parent){
	timer=new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(flush()));
    timer->setInterval(250);
    last=NULL;
}
Clock::~Clock(){
	delete timer;
	if(last!=NULL)	delete last;
}

void Clock::activate(){
	timer->start();
	this->flush();
	if(last!=NULL)
		delete last;
	last=NULL;
}

void Clock::deactivate(){
	timer->stop();
}
void Clock::flush(){
	QDateTime *now=new QDateTime(QDateTime::currentDateTime());
	if(now->toString("z").toInt()>500)
		display(now->toString("hh:mm:ss"));
	else
		display(now->toString("hh mm ss"));
    if(last!=NULL){
		if(now->toTime_t()-last->toTime_t()>10000)
			emit timeMutated();
		delete last;
	}
	last=now;
}
