#include "EventServer.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "Logger.h"
#include "HaltDialog.h"
#include "GPIOAdapter.h"
EventServer::EventServer(QObject *parent) : QObject(parent){
	server=new QLocalServer(this);
	connect(server,SIGNAL(newConnection()),this,SLOT(newConnection()));
	if(QFile("/tmp/FaceEvent").exists())
		server->removeServer("/tmp/FaceEvent");
	server->listen("/tmp/FaceEvent");
	QFile("/tmp/FaceEvent").setPermissions((QFile::Permission)0x0777);
}
EventServer::~EventServer(){
	server->close();
	delete server;
}
void EventServer::newConnection(){
	QLocalSocket* socket=server->nextPendingConnection();
	connect(socket,SIGNAL(disconnected()),socket,SLOT(deleteLater()));
	connect(socket,SIGNAL(readyRead()),this,SLOT(eventReceived()));
}
void EventServer::eventReceived(){
	QLocalSocket* socket=static_cast<QLocalSocket*>(sender());
	QJsonParseError error;
	QJsonDocument doc=QJsonDocument::fromJson(socket->readLine(),&error);
	if(error.error!=QJsonParseError::NoError)
		return;
	QJsonObject event=doc.object();
	QString eventName=event.value("act").toString();
	if(eventName.isEmpty()) return;
	else if(eventName=="logout")
		Logger::logout(event.take("content").toString(),event.value("type").toString());
	else if(eventName=="triggerHalt"){
		if(event.value("beep").toBool(false))
			GPIOAdapter::beep(2);
		HaltDialog::getInstance()->halt(event.value("delay").toInt(60),event.value("beep").toBool(false));
	}else if(eventName=="triggerReboot"){
		if(event.value("beep").toBool(false))
			GPIOAdapter::beep(2);
		HaltDialog::getInstance()->reboot(event.value("delay").toInt(60),event.value("beep").toBool(false));
    }else if(eventName=="clearLogWidget"){
        Logger::clearWidget();
    }else{
		Logger::warning("未知的远程命令:"+eventName);
	}
}
