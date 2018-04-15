#include "Beeper.h"
#include <QTimerEvent>
#include <QDebug>
#include <wiringPi.h>
bool Beeper::pinInited=false;
const int Beeper::PIN=4;
Beeper::Beeper(QObject *parent, int times, int delay, int interval) :
    QObject(parent),times(times){
    if(!Beeper::pinInited) pinMode(Beeper::PIN,OUTPUT);
    this->intervalTimer=new QTimer(this);
    this->intervalTimer->setSingleShot(true);
    this->intervalTimer->setInterval(interval);

    this->delayTimer=new QTimer(this);
    this->delayTimer->setSingleShot(true);
    this->delayTimer->setInterval(delay);

    connect(intervalTimer,SIGNAL(timeout()),this,SLOT(beepUp()));
    connect(delayTimer,SIGNAL(timeout()),this,SLOT(beepShut()));
}
Beeper::~Beeper(){
    digitalWrite(Beeper::PIN,0);
    delete this->delayTimer;
    delete this->intervalTimer;

}
void Beeper::shutup(){
    delayTimer->stop();
    intervalTimer->stop();
    this->_times=-1;
    digitalWrite(Beeper::PIN,0);
}

void Beeper::beep(int times){
    if(times>0) this->times=times;
    _times=this->times;
    this->beepUp();
}

void Beeper::beepUp(){
     digitalWrite(Beeper::PIN,1);
     intervalTimer->stop();
     delayTimer->start();
}

void Beeper::beepShut(){
     digitalWrite(Beeper::PIN,0);
     delayTimer->stop();
     if(--_times)
         intervalTimer->start();
}
