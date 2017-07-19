#include "GPIOAdapter.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
GPIOAdapter* GPIOAdapter::instance=NULL;

GPIOAdapter* GPIOAdapter::getInstance(){
	if(GPIOAdapter::instance==NULL)
		GPIOAdapter::instance=new GPIOAdapter();
	return GPIOAdapter::instance;
}
GPIOAdapter::GPIOAdapter(QObject *parent) : QObject(parent){
	client=new QLocalSocket(this);
	client->setServerName("/tmp/GPIOServer");
	client->connectToServer();
	client->waitForConnected();
	connect(client,SIGNAL(readyRead()),this,SLOT(msgReceived()));
}
void GPIOAdapter::beep(int times,int interval){
	QJsonArray list;
	QJsonObject tSetup,tUp,tDown,tDelay;
	tSetup.insert("act","setupPin");
	tSetup.insert("pin",16);
	tSetup.insert("type","OUT");
	list.append(QJsonValue(tSetup));
	tUp.insert("act","writePin");
	tUp.insert("pin",16);
	tDown=QJsonObject(tUp);
	tUp.insert("value",1);
	tDown.insert("value",0);

	tDelay.insert("act","delay");
	tDelay.insert("value",interval);
	for(int i=0;i<times;i++){
		list.append(QJsonValue(tUp));
		list.append(QJsonValue(tDelay));
		list.append(QJsonValue(tDown));
		list.append(QJsonValue(tDelay));
	}
	GPIOAdapter::getInstance()->sendList(list);
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
