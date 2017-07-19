#ifndef GPIOADAPTER_H
#define GPIOADAPTER_H

#include <QObject>
#include <QLocalSocket>
class GPIOAdapter : public QObject
{
	Q_OBJECT
public:
	explicit GPIOAdapter(QObject *parent = 0);
	static GPIOAdapter* getInstance();
	static void beep(int times=3, int interval=100);
	void sendList(QJsonArray &l);
signals:
	void LCDBrightnessReturned(int);
public slots:
	void setLCDBrightness(int brightness);
	void getLCDBrightness();
private slots:
	void msgReceived();
private:
	void send(const QJsonObject &data);
	void parseEvent(const QJsonObject &event);
	static GPIOAdapter* instance;
	QLocalSocket* client;
};

#endif // GPIOADAPTER_H
