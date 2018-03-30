#include "Logger.h"
#include "WebsocketServer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMutex>
#include <QMutexLocker>
WebsocketServer* WebsocketServer::m_instance=NULL;
WebsocketServer* WebsocketServer::instance(){
    static QMutex insMutex;
    if(!WebsocketServer::m_instance){
        QMutexLocker locker(&insMutex);
        if(!WebsocketServer::m_instance)
            WebsocketServer::m_instance=new WebsocketServer(NULL);
    }
    return WebsocketServer::m_instance;
}

WebsocketServer::WebsocketServer(QObject *parent):
    QWebSocketServer("websocket",QWebSocketServer::NonSecureMode,parent)
{
    this->registerObject("server",this);
    if(this->listen(QHostAddress::Any,6600))
        Logger::info(QString("[WS]启动正常"));
    else
        Logger::error(QString("[WS]%1").arg(this->errorString()));
    connect(this,SIGNAL(newConnection()),this,SLOT(newClientConnected()));
}
WebsocketServer::~WebsocketServer(){
    if(!clients.isEmpty())
        foreach(QWebSocket* client,clients)
            client->close();
    this->close();
}

bool WebsocketServer::registerObject(QString name,QObject* obj){
    if(objMap.contains(name)) return false;
    objMap.insert(name,obj);
    return true;
}

bool WebsocketServer::unregisterObject(QString name){
    if(!objMap.contains(name)) return false;
    objMap.remove(name);
    return true;
}

void WebsocketServer::newClientConnected(){
    QWebSocket* client=this->nextPendingConnection();
    clients<<client;
    connect(client,SIGNAL(textMessageReceived(QString)),this,SLOT(messageReceived(QString)));
    connect(client,SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
    emit activeClientsChanged(clients.length());
}
void WebsocketServer::messageReceived(QString text){
    QWebSocket* client=qobject_cast<QWebSocket*>(sender());
    QJsonObject recvObj=QJsonDocument::fromJson(text.toUtf8()).object();
    QJsonObject retnObj;
    if(recvObj.contains("id")) retnObj.insert("id",recvObj.value("id"));
    QStringList cmdList=recvObj.value("method").toString().split('.');
    if(cmdList.length()<2){
        retnObj.insert("status",false);
        retnObj.insert("error",QString("Invalid object.method was called in method:%1").arg(recvObj.value("method").toString()));
    }else if(!objMap.contains(cmdList[0])){
        retnObj.insert("status",false);
        retnObj.insert("error",QString("Cannot find object:%1").arg(cmdList[0]));
    }else{
        QObject* host=objMap.value(cmdList[0]);
        const QMetaObject* metaObj=host->metaObject();
        bool hasParams=recvObj.contains("params");
        QString methodName=QString("%1(%2)").arg(cmdList[1]).arg(hasParams?"QVariantMap":"");
        int methodIndex=metaObj->indexOfMethod(methodName.toStdString().c_str());
        if(methodIndex>0){
            QJsonValue ret;
            QMetaMethod method=metaObj->method(methodIndex);
            bool status=false;
            if(hasParams)
                status=method.invoke(host,Q_RETURN_ARG(QJsonValue,ret),Q_ARG(QVariant,recvObj.value("params").toVariant()));
            else
                status=method.invoke(host,Q_RETURN_ARG(QJsonValue,ret),QGenericArgument());
            retnObj.insert("status",status);
            if(!status) retnObj.insert("error","Cannot Invoke the method!");
            retnObj.insert("return",ret);
        }else{
            retnObj.insert("status","false");
            retnObj.insert("error",QString("Cannot Find method %1").arg(recvObj.value("method").toString()));
        }
    }
    client->sendTextMessage(QJsonDocument(retnObj).toJson(QJsonDocument::Compact));
}
void WebsocketServer::clientDisconnected(){
    QWebSocket* client=qobject_cast<QWebSocket*>(sender());
    clients.removeAt(clients.indexOf(client));
    emit activeClientsChanged(clients.length());
}

QJsonValue WebsocketServer::getAvaliableMethods(){
    QJsonArray retn;
    foreach(QString objName,objMap.keys()){
        const QMetaObject* metaObj=objMap.value(objName)->metaObject();
        for(int i=0;i<metaObj->methodCount();i++){
            QMetaMethod theMethod=metaObj->method(i);
            retn.append(QString("%1.%2").arg(objName,QString(theMethod.name())));
        }
    }
    return retn;
}
