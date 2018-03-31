#include "Logger.h"
#include "WebsocketServer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
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
    QJsonParseError parseError;
    QJsonObject recvObj=QJsonDocument::fromJson(text.toUtf8(),&parseError).object();
    QJsonObject respObj;
    if(parseError.error!=QJsonParseError::NoError){
        respObj.insert("status",false);
        respObj.insert("error",QString("ParseError:%1").arg(parseError.errorString()));
    }else{
        respObj=this->invokeMethod(recvObj);
    }
    respObj.insert("type","response");
    client->sendTextMessage(QJsonDocument(respObj).toJson(QJsonDocument::Compact));
}
QJsonObject WebsocketServer::invokeMethod(QJsonObject reqObj){
    QJsonObject retnObj;
    if(reqObj.contains("id")) retnObj.insert("id",reqObj.value("id"));
    QStringList cmdList=reqObj.value("method").toString().split('.');
    if(cmdList.length()<2){
        retnObj.insert("status",false);
        retnObj.insert("error",QString("Invalid object.method was called in method:%1").arg(reqObj.value("method").toString()));
    }else if(!objMap.contains(cmdList[0])){
        retnObj.insert("status",false);
        retnObj.insert("error",QString("Cannot find object:%1").arg(cmdList[0]));
    }else{
        QObject* host=objMap.value(cmdList[0]);
        const QMetaObject* metaObj=host->metaObject();
        /**
         * @brief hasParams
         * 有params参数,并且params参数不是空(Array非空和Object非空)
         */
        bool hasParams=reqObj.contains("params")&&!(reqObj.value("params").type()==QJsonValue::Array?reqObj.value("params").toArray().isEmpty():reqObj.value("params").toObject().isEmpty());
        QString methodName=QString("%1(%2)").arg(cmdList[1]).arg(hasParams?"QJsonValue":"");
        int methodIndex=metaObj->indexOfMethod(methodName.toStdString().c_str());
        if(methodIndex>0){
            QJsonValue ret;
            QMetaMethod method=metaObj->method(methodIndex);
            bool status=false;
            if(hasParams)
                status=method.invoke(host,Q_RETURN_ARG(QJsonValue,ret),Q_ARG(QJsonValue,reqObj.value("params")));
            else
                status=method.invoke(host,Q_RETURN_ARG(QJsonValue,ret),QGenericArgument());
            retnObj.insert("status",status);
            if(!status) retnObj.insert("error","Failed to invoke the method!");
            retnObj.insert("return",ret);
        }else{
            retnObj.insert("status","false");
            retnObj.insert("error",QString("Cannot Find method %1").arg(reqObj.value("method").toString()));
        }
    }
    return retnObj;
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
QJsonValue WebsocketServer::multiCall(QJsonValue params){
    QJsonArray recvQueue=params.toArray();
    QJsonArray respQueue;
    foreach(QJsonValue item,recvQueue){
        respQueue.append(this->invokeMethod(item.toObject()));
    }
    return respQueue;
}
