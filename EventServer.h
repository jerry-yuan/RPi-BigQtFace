#ifndef EVENTSERVER_H
#define EVENTSERVER_H

#include <QObject>
#include <QLocalServer>
class EventServer : public QObject
{
	Q_OBJECT
public:
	explicit EventServer(QObject *parent = 0);
	~EventServer();
signals:

public slots:
private slots:
	void newConnection();
	void eventReceived();
private:
	QLocalServer *server;
};

#endif // EVENTSERVER_H
