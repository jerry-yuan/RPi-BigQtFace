#include "GPIOAdapter.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include "EventServer.h"
GPIOAdapter* GPIOAdapter::m_instance=NULL;

GPIOAdapter* GPIOAdapter::instance(){
    static QMutex insMutex;
    if(!GPIOAdapter::m_instance){
        QMutexLocker locker(&insMutex);
        if(!GPIOAdapter::m_instance)
            GPIOAdapter::m_instance=new GPIOAdapter();
    }
    return GPIOAdapter::m_instance;
}
GPIOAdapter::GPIOAdapter(QObject *parent) : QObject(parent){
	client=new QLocalSocket(this);
	client->setServerName("/tmp/GPIOServer");
	client->connectToServer();
	client->waitForConnected();
	connect(client,SIGNAL(readyRead()),this,SLOT(msgReceived()));

    EventServer::instance()->addMethod(this,"beep","doBeep");
}
void GPIOAdapter::doBeep(QVariantHash params){
    int times=params.value("times").toInt();
    int delay=params.value("delay").toInt();

    times=times>0?times:0;
    delay=delay>0?delay:0;
    if(times==0||delay==0) return;
    QJsonArray multicall;
    //初始化Pin
    QJsonObject tSetup;
    tSetup.insert("act","setupPin");
    tSetup.insert("pin",16);
    tSetup.insert("type","OUT");
    multicall.append(QJsonValue(tSetup));
    //循环times次
    QJsonObject loop;
    loop.insert("act","loop");
    loop.insert("times",times);
    //多步指令
    QJsonObject loopAction;
    loopAction.insert("act","multiCall");
    //循环体
    QJsonArray loopBody;
    QJsonObject tUp,tDown,tDelay;
    tUp.insert("act","writePin");
    tUp.insert("pin",16);
    tDown=QJsonObject(tUp);
    tUp.insert("value",1);
    tDown.insert("value",0);

    tDelay.insert("act","delay");
    tDelay.insert("value",delay);
    loopBody.append(QJsonValue(tUp));
    loopBody.append(QJsonValue(tDelay));
    loopBody.append(QJsonValue(tDown));
    loopBody.append(QJsonValue(tDelay));

    loopAction.insert("list",loopBody);

    loop.insert("action",loopAction);

    multicall.append(QJsonValue(loop));
    this->sendList(multicall);
}

void GPIOAdapter::beep(int times,int delay){
    QVariantHash params;
    params.insert("times",times);
    params.insert("delay",delay);
    GPIOAdapter::instance()->doBeep(params);
}
void GPIOAdapter::sendList(QJsonArray &l){
	QJsonObject msg;
	msg.insert("act",QJsonValue("multiCall"));
	msg.insert("list",l);
	send(msg);
}
void GPIOAdapter::getLCDBrightness(){
	QJsonObject data;
	data.insert("act","LCD.getBrightness");
	send(data);
}

void GPIOAdapter::setLCDBrightness(int brightness){
	brightness=qMax(11,brightness);
	brightness=qMin(255,brightness);
	QJsonObject data;
	data.insert("act","LCD.setBrightness");
	data.insert("value",brightness);
	send(data);
}
void GPIOAdapter::send(const QJsonObject &data){
	if(!client->isOpen())
		client->connectToServer();
    qDebug()<<data;
	client->write(QJsonDocument(data).toJson(QJsonDocument::Compact));
}
void GPIOAdapter::msgReceived(){
	QJsonObject event=QJsonDocument::fromJson(client->readAll()).object();
	this->parseEvent(event);
}
void GPIOAdapter::parseEvent(const QJsonObject &event){
	QString eventName=event.value("event").toString();
	if(eventName=="LCD.getBrightnessReturn"){
		emit LCDBrightnessReturned(event.value("value").toInt(255));
	}else if(eventName=="multiCallReturn"){
		foreach(QJsonValue val,event.value("list").toArray()){
			if(val.isObject())
				parseEvent(val.toObject());
		}
	}
}
