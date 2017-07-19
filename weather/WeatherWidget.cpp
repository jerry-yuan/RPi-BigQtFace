#include "Consts.h"
#include "WeatherWidget.h"
#include "ui_WeatherWidget.h"
#include "Logger.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QNetworkReply>
WeatherWidget::WeatherWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WeatherWidget)
{
	ui->setupUi(this);
	//初始化界面
	ui->currentIcon->setPixmap(QPixmap());
	ui->currentDesc->setText("-");
	ui->currentDeg->setText("--℃");
	ui->todayRange->setText("--℃~--℃");
	ui->windDirect->setDegree(0);
	ui->windSpeed->setText("--");
	//初始化网络管理器
	manager=new QNetworkAccessManager(this);
	//初始化请求
	APIKey="7f8c4da3ce9849ffb2134f075201c45a";
	LocationKey="1796956";
	this->setupRequest();
	timer=new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(flushAll()));
	connect(ui->freshButton,SIGNAL(clicked()),this,SLOT(flushAll()));
	connect(ui->f24hFresh,SIGNAL(clicked()),this,SLOT(flushForecast24h()));
	connect(ui->f4dFresh,SIGNAL(clicked()),this,SLOT(flushForecast4d()));
	connect(ui->airFresh,SIGNAL(clicked()),this,SLOT(flushAir()));

}
void WeatherWidget::setupRequest(){
	//当前状态数据
	cC.setHeader(QNetworkRequest::UserAgentHeader,"Dalvik/2.1.0 (Linux; U; Android 7.0; KNT-UL10 Build/HUAWEIKNT-UL10)");
	cC.setUrl(QUrl(QString("http://api.accuweather.com/currentconditions/v1/%1.json?apikey=%2&language=zh-CN&details=true").arg(LocationKey,APIKey)));
	f24h.setHeader(QNetworkRequest::UserAgentHeader,"Dalvik/2.1.0 (Linux; U; Android 7.0; KNT-UL10 Build/HUAWEIKNT-UL10)");
	f24h.setUrl(QUrl(QString("http://api.accuweather.com/forecasts/v1/hourly/24hour/%1?apikey=%2&language=zh-CN&details=true").arg(LocationKey,APIKey)));
	f4d.setHeader(QNetworkRequest::UserAgentHeader,"Dalvik/2.1.0 (Linux; U; Android 7.0; KNT-UL10 Build/HUAWEIKNT-UL10)");
	f4d.setUrl(QUrl(QString("http://api.accuweather.com/forecasts/v1/daily/5day/%1?apikey=%2&language=zh-CN&details=true").arg(LocationKey,APIKey)));
	air.setHeader(QNetworkRequest::UserAgentHeader,"Dalvik/2.1.0 (Linux; U; Android 7.0; KNT-UL10 Build/HUAWEIKNT-UL10)");
	air.setUrl(QUrl(QString("http://api.accuweather.com/airquality/v1/observations/%1.json?apikey=%2&language=zh-CN").arg(LocationKey,APIKey)));
}

WeatherWidget::~WeatherWidget(){
	delete ui;
}
qreal WeatherWidget::toCelsius(qreal f){
	return (f-32)/1.8;
}
void WeatherWidget::activate(){
	flushAll();
}
void WeatherWidget::deactivate(){
	timer->stop();
}

//刷新全部
void WeatherWidget::flushAll(){
	//return;
	timer->stop();
	flushCurrentCondition();
	flushForecast24h();
	flushForecast4d();
	flushAir();
	timer->start(900000);

}
//刷新当前天气
void WeatherWidget::flushCurrentCondition(){
	cCReply=manager->get(cC);
	QTimer::singleShot(NET_WAITING_TIME,cCReply,SLOT(abort()));
	connect(cCReply,SIGNAL(finished()),this,SLOT(cCReceived()));
}
//刷新24小时预报
void WeatherWidget::flushForecast24h(){
	ui->f24hFresh->setEnabled(false);
	f24hReply=manager->get(f24h);
	QTimer::singleShot(NET_WAITING_TIME,f24hReply,SLOT(abort()));
	connect(f24hReply,SIGNAL(finished()),this,SLOT(f24hReceived()));
}
//刷新4日预报
void WeatherWidget::flushForecast4d(){
	f4dReply=manager->get(f4d);
	QTimer::singleShot(NET_WAITING_TIME,f4dReply,SLOT(abort()));
	connect(f4dReply,SIGNAL(finished()),this,SLOT(f4dReceived()));
}
//刷新空气质量
void WeatherWidget::flushAir(){
	airReply=manager->get(air);
	QTimer::singleShot(NET_WAITING_TIME,airReply,SLOT(abort()));
	connect(airReply,SIGNAL(finished()),this,SLOT(airReceived()));
}

//当前状况API接收
void WeatherWidget::cCReceived(){
	disconnect(cCReply,SIGNAL(readyRead()),this,SLOT(cCReceived()));
	if(cCReply->error()==QNetworkReply::OperationCanceledError){
		if(!networkStatus) return;
		Logger::error("拉取当前天气超时...");
		if(cCRetried++<5)
			QTimer::singleShot(5000,this,SLOT(flushCurrentCondition()));
	}
	cCRetried=0;
	if(cCReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=200){
		if(networkStatus){
			networkStatus=false;
			emit networkFailed();
			Logger::warning("疑似网络异常!");
		}
		return;
	}
	QJsonDocument qdoc=QJsonDocument::fromJson(cCReply->readAll());
	QJsonObject data=qdoc.array().at(0).toObject();
	ui->currentIcon->setPixmap(QPixmap(QString(":/weather/icons/%1.png").arg(QString::number(data.take("WeatherIcon").toInt()))));
	ui->currentDesc->setText(data.take("WeatherText").toString());
	ui->currentDeg->setText(QString("%1℃").arg(QString::number((int)data.take("Temperature").toObject().take("Metric").toObject().take("Value").toDouble())));
	QJsonObject wind=data.take("Wind").toObject();
	ui->windDirect->setDegree(wind.take("Direction").toObject().take("Degrees").toInt());
	QJsonObject windSpeed=wind.take("Speed").toObject().take("Metric").toObject();
	ui->windSpeed->setText(QString("%1%2").arg(QString::number(windSpeed.take("Value").toDouble()),windSpeed.take("Unit").toString()));
	ui->stackedWidget->setCurrentIndex(1);
}
//24小时预报API接收
void WeatherWidget::f24hReceived(){
	disconnect(f24hReply,SIGNAL(finished()),this,SLOT(f24hReceived()));
	ui->f24hFresh->setEnabled(true);;
	if(f24hReply->error()==QNetworkReply::OperationCanceledError){
		if(!networkStatus) return;
		Logger::error("拉取24小时预报超时...");
		if(f24hRetried++<5)
			QTimer::singleShot(5000,this,SLOT(flushForecast24h()));
	}else if(f24hReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=200){
		if(networkStatus){
			networkStatus=false;
			emit networkFailed();
			Logger::warning("疑似网络异常!");
		}
		return;
	}

	QJsonDocument qdoc=QJsonDocument::fromJson(f24hReply->readAll());
	QJsonArray forecasts=qdoc.array();
	for(int i=0;i<forecasts.count();i++){
		QJsonObject temp=forecasts.at(i).toObject();
		ui->f24hforecastContent->findChild<QLabel*>(QString("f24Time_%1").arg(QString::number(i)))->setText(QDateTime::fromTime_t(temp.take("EpochDateTime").toInt()).toString("hh:mm"));
		ui->f24hforecastContent->findChild<QLabel*>(QString("f24Icon_%1").arg(QString::number(i)))->setPixmap(QPixmap(QString(":/weather/icons/%1.png").arg(QString::number(temp.take("WeatherIcon").toInt()))));
		ui->f24hforecastContent->findChild<QLabel*>(QString("f24Desc_%1").arg(QString::number(i)))->setText(temp.take("IconPhrase").toString());
		ui->f24hforecastContent->findChild<QLabel*>(QString("f24Temp_%1").arg(QString::number(i)))->setText(QString("%1℃").arg(QString::number(toCelsius(temp.take("Temperature").toObject().take("Value").toInt()),'f',0)));
	}
	ui->stackedWidget->setCurrentIndex(1);
}
//4天预报接收
void WeatherWidget::f4dReceived(){
	disconnect(f4dReply,SIGNAL(finished()),this,SLOT(f4dReceived()));
	if(f4dReply->error()==QNetworkReply::OperationCanceledError){
		if(!networkStatus) return;
		Logger::error("拉取24小时预报超时...");
		if(f4dRetried++<5)
			QTimer::singleShot(5000,this,SLOT(flushForecast4d()));
	}else if(f4dReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=200){
		if(networkStatus){
			networkStatus=false;
			emit networkFailed();
			Logger::warning("疑似网络异常!");
		}
		return;
	}
	QJsonObject data=QJsonDocument::fromJson(f4dReply->readAll()).object();
	QJsonArray f4dForecast=data.take("DailyForecasts").toArray();
	QJsonObject today=f4dForecast.at(0).toObject();
	QJsonObject todayTemp=today.take("Temperature").toObject();
	QString todayMinTemp=QString("%1℃").arg(QString::number(toCelsius(todayTemp.take("Minimum").toObject().take("Value").toDouble()),'f',0));
	QString todayMaxTemp=QString("%1℃").arg(QString::number(toCelsius(todayTemp.take("Maximum").toObject().take("Value").toDouble()),'f',0));
	ui->todayRange->setText(QString("%1~%2").arg(todayMinTemp,todayMaxTemp));
	for(int i=1;i<f4dForecast.count();i++){
		QJsonObject dayForecast=f4dForecast.at(i).toObject();
		QJsonObject day=dayForecast.take("Day").toObject();
		QJsonObject night=dayForecast.take("Night").toObject();
		QJsonObject temp=dayForecast.take("Temperature").toObject();
		QString minTemp=QString("%1℃").arg(QString::number(toCelsius(temp.take("Minimum").toObject().take("Value").toDouble()),'f',0));
		QString maxTemp=QString("%1℃").arg(QString::number(toCelsius(temp.take("Maximum").toObject().take("Value").toDouble()),'f',0));
		ui->detailArea->findChild<QLabel*>(QString("f4dDate_%1").arg(QString::number(i-1)))->setText(QDateTime::fromTime_t(dayForecast.take("EpochDate").toInt()).toString("MM月dd日"));
		ui->detailArea->findChild<QLabel*>(QString("f4dTemp_%1").arg(QString::number(i-1)))->setText(minTemp+"/"+maxTemp);

		ui->detailArea->findChild<QLabel*>(QString("f4dDayIcon_%1").arg(QString::number(i-1)))->setPixmap(QPixmap(QString(":/weather/icons/%1.png").arg(QString::number(day.take("Icon").toInt()))));
		ui->detailArea->findChild<QLabel*>(QString("f4dNightIcon_%1").arg(QString::number(i-1)))->setPixmap(QPixmap(QString(":/weather/icons/%1.png").arg(QString::number(night.take("Icon").toInt()))));
	}
	ui->stackedWidget->setCurrentIndex(1);
}
//空气质量接收
void WeatherWidget::airReceived(){
	disconnect(airReply,SIGNAL(finished()),this,SLOT(airReceived()));
	if(airReply->error()==QNetworkReply::OperationCanceledError){
		if(!networkStatus) return;
		Logger::error("拉取24小时预报超时...");
		if(airRetried++<5)
			QTimer::singleShot(5000,this,SLOT(flushAir()));
	}else if(airReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=200){
		if(networkStatus){
			networkStatus=false;
			emit networkFailed();
			Logger::warning("疑似网络异常!");
		}
		return;
	}
	QJsonObject data=QJsonDocument::fromJson(airReply->readAll()).object();
	ui->airIndex->setText(QString::number(data.take("Index").toInt()));
	ui->airPM2_5->setText(QString::number(data.take("ParticulateMatter2_5").toInt()));
	ui->airPM10->setText(QString::number(data.take("ParticulateMatter10").toInt()));
	ui->airO3->setText(QString::number(data.take("Ozone").toInt()));
	ui->airCO->setText(QString::number(data.take("CarbonMonoxide").toInt()));
	ui->airNO2->setText(QString::number(data.take("NitrogenDioxide").toInt()));
	ui->airSO2->setText(QString::number(data.take("SulfurDioxide").toInt()));
}
