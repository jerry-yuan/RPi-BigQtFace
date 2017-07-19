#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QWidget>

namespace Ui {
	class Temperature;
	}

class Temperature : public QWidget
{
	Q_OBJECT

public:
	explicit Temperature(QWidget *parent = 0);
	~Temperature();
private slots:
	void addData();
private:
	qreal getData();
	Ui::Temperature *ui;
	qreal maxTemp,minTemp;
	QList<qreal> tempList;
	QList<qreal> timeList;
	QTimer* timer;
};

#endif // TEMPERATURE_H
