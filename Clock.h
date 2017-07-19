#ifndef CLOCK_H
#define CLOCK_H
#include <QLCDNumber>
#include <QObject>

class Clock : public QLCDNumber{
	Q_OBJECT
public:
	explicit Clock(QWidget *parent);
	~Clock();
signals:
	void timeMutated();
public slots:
	void activate();
	void deactivate();
private slots:
	void flush();
private:
	QDateTime *last;
	QTimer* timer;
};
#endif // CLOCK_H
