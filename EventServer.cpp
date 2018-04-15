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
#include <QMetaMethod>
EventServer* EventServer::m_instance=NULL;
EventServer* EventServer::instance(){
    static QMutex insMutex;
    if(!EventServer::m_instance){
        QMutexLocker locker(&insMutex);
        if(!EventServer::m_instance)
            EventServer::m_instance=new EventServer();
    }
    return EventServer::m_instance;
}
void EventServer::addMethod(QObject *receiver, const QString actName,const QString methodName){
    QString tMethodName=methodName;
    if(tMethodName.isNull()) tMethodName=actName;
    if(tMethodName.indexOf("(QVariantHash)")<0)
        tMethodName=QString("%1(QVariantHash)").arg(tMethodName);
    const QMetaObject* mObjects=receiver->metaObject();
    int methodId=mObjects->indexOfMethod(tMethodName.toStdString().c_str());
    if(methodId<0){
        qCritical()<<QString("[ES]没有在%1中找到方法%2").arg(mObjects->className(),tMethodName);
        return;
    }
    InvokeObject t;
    t.method=mObjects->method(methodId);
    t.target=receiver;
    this->methods.insert(actName,t);
}

EventServer::EventServer(QObject *parent) : QObject(parent){
	server=new QLocalServer(this);
	connect(server,SIGNAL(newConnection()),this,SLOT(newConnection()));
	if(QFile("/tmp/FaceEvent").exists())
		server->removeServer("/tmp/FaceEvent");
	server->listen("/tmp/FaceEvent");
    QFile("/tmp/FaceEvent").setPermissions((QFile::Permission)0x0777);
    EventServer::addMethod(this,"logout");
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
    //没有错误
    if(error.error!=QJsonParseError::NoError) return;
    //取出事件主体
    QJsonObject event=doc.object();
    QString eventName=event.take("act").toString();
    //空事件
	if(eventName.isEmpty()) return;
    //事件参数
    QVariantHash params=event.toVariantHash();
    //注册过的事件
    if(methods.keys().contains(eventName)){
        InvokeObject t=methods.value(eventName);
        t.method.invoke(t.target,Q_ARG(QVariantHash,params));
    }else{
        qWarning("未知的远程命令:%s",qPrintable(eventName));
    }
}

void EventServer::logout(QVariantHash params){
    qDebug()<<qPrintable(params.value("content").toString());
}
