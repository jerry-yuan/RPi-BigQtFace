#include "BeeperThread.h"
#include <wiringPi.h>
quint8 BeeperThread::pin=4;
bool BeeperThread::pinInited=false;
BeeperThread::BeeperThread(QObject *parent):QThread(parent){
    this->interval=100;
    this->delay=100;
    this->times=0;
    if(!BeeperThread::pinInited){
        BeeperThread::pinInited=true;
        pinMode(BeeperThread::pin,OUTPUT);
    }
}
BeeperThread::~BeeperThread(){
    digitalWrite(BeeperThread::pin,LOW);
}
void BeeperThread::beep(quint32 times){
    if(this->isRunning())
        this->wait();
    this->times=times;
    this->start(TimeCriticalPriority);
}

void BeeperThread::run(){
    while(this->times>0){

        digitalWrite(BeeperThread::pin,HIGH);
        this->msleep(this->delay);
        digitalWrite(BeeperThread::pin,LOW);
        this->msleep(this->interval);
        this->times--;
        if(this->thread()->isInterruptionRequested())
            break;
    }
}
void BeeperThread::setDelay(quint32 delay){
    this->delay=delay;
}

void BeeperThread::setInterval(quint32 interval){
    this->interval=interval;
}

