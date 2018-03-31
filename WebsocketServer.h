#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QWebSocket>
#include <QWebSocketServer>
#include <QJsonValue>
#include <QTimer>
class WebsocketServer : public QWebSocketServer{
    Q_OBJECT
public:
    WebsocketServer(QObject* parent=0);
    ~WebsocketServer();
    static WebsocketServer* instance();
    bool registerObject(QString name,QObject* obj);
    bool unregisterObject(QString name);
    Q_INVOKABLE QJsonValue getAvaliableMethods();
    Q_INVOKABLE QJsonValue multiCall(QJsonValue params);
signals:
    void activeClientsChanged(int);
private slots:
    void newClientConnected();
    void messageReceived(QString text);
    void clientDisconnected();
private:
    QJsonObject invokeMethod(QJsonObject reqObj);
    static WebsocketServer* m_instance;
    QList<QWebSocket*> clients;
    QMap<QString,QObject*> objMap;
};

#endif // WEBSOCKETSERVER_H
