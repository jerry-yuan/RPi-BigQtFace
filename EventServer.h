#ifndef EVENTSERVER_H
#define EVENTSERVER_H

#include <QObject>
#include <QLocalServer>
#include <QHash>
#include <QMetaMethod>
class EventServer : public QObject
{
	Q_OBJECT
public:
    static EventServer* instance();

	explicit EventServer(QObject *parent = 0);
	~EventServer();

    void addMethod(QObject *receiver, const QString actName, const QString methodName=QString());
signals:

public slots:
    void logout(QVariantHash params);
private slots:
	void newConnection();
    void eventReceived();
private:
    struct InvokeObject{
        QObject* target;
        QMetaMethod method;
    };

    static EventServer* m_instance;
    QHash<QString,InvokeObject> methods;
	QLocalServer *server;
};

#endif // EVENTSERVER_H
