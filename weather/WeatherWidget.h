#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWidget>

namespace Ui {
	class WeatherWidget;
	}

class WeatherWidget : public QWidget
{
	Q_OBJECT

public:
	explicit WeatherWidget(QWidget *parent = 0);
	~WeatherWidget();
signals:
	void networkFailed();
public slots:
	void activate();
	void deactivate();

	void flushAll();
	void flushCurrentCondition();
	void flushForecast24h();
	void flushForecast4d();
	void flushAir();
private slots:
	void cCReceived();
	void f24hReceived();
	void f4dReceived();
	void airReceived();
private:
	qreal toCelsius(qreal f);
	void setupRequest();
	bool networkStatus;
	QString APIKey;
	QString LocationKey;
	QNetworkAccessManager *manager;
	QNetworkRequest cC;
	QNetworkRequest f24h;
	QNetworkRequest f4d;
	QNetworkRequest air;
	QNetworkReply* f24hReply;
	QNetworkReply* cCReply;
	QNetworkReply* f4dReply;
	QNetworkReply* airReply;
	int cCRetried;
	int f24hRetried;
	int f4dRetried;
	int airRetried;
	QTimer *timer;
	Ui::WeatherWidget *ui;
};

#endif // WEATHERWIDGET_H
