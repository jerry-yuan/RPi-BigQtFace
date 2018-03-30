#ifndef NETWORKSPEED_H
#define NETWORKSPEED_H

#include <QWidget>
#include <QJsonValue>
namespace Ui {
	class NetworkSpeed;
	}

class NetworkSpeed : public QWidget
{
	Q_OBJECT

public:
	explicit NetworkSpeed(QWidget *parent = 0);
	~NetworkSpeed();
    Q_INVOKABLE QJsonValue getNetworkDev();
private slots:
	void calcSpeed();
private:
	QTimer* timer;
    quint64 uploaded;
    quint64 downloaded;
    quint64 dSpeed;
    quint64 uSpeed;
	//long unsigned uploaded;
	//long unsigned downloaded;
    QString bitCountToString(quint64 sum);
	Ui::NetworkSpeed *ui;

};

#endif // NETWORKSPEED_H
