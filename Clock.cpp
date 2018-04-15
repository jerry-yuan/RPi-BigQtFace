#include "Clock.h"
#include <QTimer>
#include <QLCDNumber>
#include <QDateTime>
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
        if(last->secsTo(*now)>1000)
			emit timeMutated();
		delete last;
	}
    last=now;
}
