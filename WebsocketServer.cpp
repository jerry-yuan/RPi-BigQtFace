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
    QWebSocketServer("websocket",QWebSocketServer::NonSecureMode,parent),
    timer(new QTimer(this))
{
    timer->setInterval(5000);
    this->registerObject("server",this);
    if(this->listen(QHostAddress::Any,6600))
        qDebug()<<"[WS]启动正常";
    else
        qDebug("[WS]%s",qPrintable(this->errorString()));
    connect(this,SIGNAL(newConnection()),this,SLOT(newClientConnected()));
    connect(timer,SIGNAL(timeout()),this,SLOT(dropDiedSession()));
}
WebsocketServer::~WebsocketServer(){
    if(!clients.isEmpty()){
        QMap<QWebSocket*,WSSession>::iterator i=clients.begin();
        while(i!=clients.end()){
            disconnect(i.key(),SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
            i.key()->close();
            i=clients.erase(i);
        }
    }
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
    WSSession session;
    session.actTime=QDateTime::currentDateTime();
    clients[client]=session;
    connect(client,SIGNAL(textMessageReceived(QString)),this,SLOT(messageReceived(QString)));
    connect(client,SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
    connect(client,SIGNAL(disconnected()),client,SLOT(deleteLater()));
    emit activeClientsChanged(clients.keys().length());
    if(!timer->isActive()) timer->start();
}
void WebsocketServer::messageReceived(QString text){
    QWebSocket* client=qobject_cast<QWebSocket*>(sender());
    QJsonParseError parseError;
    clients[client].actTime=QDateTime::currentDateTime();
    QJsonObject recvObj=QJsonDocument::fromJson(text.toUtf8(),&parseError).object();
    QJsonObject respObj;
    if(parseError.error!=QJsonParseError::NoError){
        respObj.insert("type",WebsocketServer::Error);
        respObj.insert("request",text);
        respObj.insert("error",QString("ParseError:%1").arg(parseError.errorString()));
    }else{
        MsgType msgType=(WebsocketServer::MsgType)respObj.value("type").toInt(WebsocketServer::Request);
        switch(msgType){
        case WebsocketServer::Request:
            respObj=this->invokeMethod(recvObj);
            respObj.insert("type",WebsocketServer::Response);
            break;
        case WebsocketServer::Response:
            break;
        case WebsocketServer::Error:
            break;
        }
    }
    if(!respObj.isEmpty())
        client->sendTextMessage(QJsonDocument(respObj).toJson(QJsonDocument::Compact));
}
QJsonObject WebsocketServer::invokeMethod(QJsonObject reqObj){
    QJsonObject retnObj;
    if(reqObj.contains("id")) retnObj.insert("id",reqObj.value("id"));
    QString methodVal=reqObj.value("method").toString("");
    if(!methodVal.isEmpty()&&methodVal.indexOf(".")<0)
        methodVal=QString("server.%1").arg(methodVal);
    QStringList cmdList=methodVal.split(".");
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
            if(method.returnType()!=QMetaType::QJsonValue){
                retnObj.insert("status","false");
                retnObj.insert("error",QString("Cannot Find method %1").arg(reqObj.value("method").toString()));
            }else{
                bool status=false;
                if(hasParams)
                    status=method.invoke(host,Q_RETURN_ARG(QJsonValue,ret),Q_ARG(QJsonValue,reqObj.value("params")));
                else
                    status=method.invoke(host,Q_RETURN_ARG(QJsonValue,ret),QGenericArgument());
                retnObj.insert("status",status);
                if(!status) retnObj.insert("error","Failed to invoke the method!");
                retnObj.insert("return",ret);
            }
        }else{
            retnObj.insert("status","false");
            retnObj.insert("error",QString("Cannot Find method %1").arg(reqObj.value("method").toString()));
        }
    }
    return retnObj;
}

void WebsocketServer::clientDisconnected(){
    QWebSocket* client=qobject_cast<QWebSocket*>(sender());
    clients.remove(client);
    emit activeClientsChanged(clients.keys().length());
}
void WebsocketServer::dropDiedSession(){
    QMap<QWebSocket*,WSSession>::iterator i=clients.begin();
    QDateTime checkTime=QDateTime::currentDateTime();
    while(i!=clients.end()){
        WSSession session=i.value();
        if(session.actTime.secsTo(checkTime)>60){
            disconnect(i.key(),SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
            i.key()->close();
            i=clients.erase(i);
            emit activeClientsChanged(clients.keys().length());
        }else{
            i++;
        }
    }
    if(clients.isEmpty())
        timer->stop();
}

QJsonValue WebsocketServer::getAvaliableMethods(){
    QJsonArray retn;
    foreach(QString objName,objMap.keys()){
        const QMetaObject* metaObj=objMap.value(objName)->metaObject();
        for(int i=0;i<metaObj->methodCount();i++){
            QMetaMethod theMethod=metaObj->method(i);
            if(theMethod.returnType()!=QMetaType::QJsonValue) continue;
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
