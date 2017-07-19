#ifndef NETWORKSPEED_H
#define NETWORKSPEED_H

#include <QWidget>

namespace Ui {
	class NetworkSpeed;
	}

class NetworkSpeed : public QWidget
{
	Q_OBJECT

public:
	explicit NetworkSpeed(QWidget *parent = 0);
	~NetworkSpeed();
private slots:
	void calcSpeed();
private:
	QTimer* timer;
	unsigned long long uploaded;
	unsigned long long downloaded;
	//long unsigned uploaded;
	//long unsigned downloaded;
	QString bitCountToString(unsigned long long sum);
	Ui::NetworkSpeed *ui;

};

#endif // NETWORKSPEED_H
