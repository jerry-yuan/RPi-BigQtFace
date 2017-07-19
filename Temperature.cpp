#include "Temperature.h"
#include "ui_Temperature.h"
#include "qcustomplot.h"
Temperature::Temperature(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Temperature)
{
	ui->setupUi(this);
	timer=new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(addData()));

	ui->diagram->addGraph();
	ui->diagram->graph(0)->setPen(QColor(255, 110, 40));

	QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
	timeTicker->setDateTimeFormat("hh:mm:ss");
	ui->diagram->xAxis->setTicker(timeTicker);
	ui->diagram->axisRect()->setupFullAxesBox();
	//ui->diagram->yAxis->setRange(10, 60);
	maxTemp=0;
	minTemp=100;
	//connect(ui->diagram->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->centralWidget->xAxis2, SLOT(setRange(QCPRange)));
	//connect(ui->diagram->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->centralWidget->yAxis2, SLOT(setRange(QCPRange)));
	ui->temp->display(getData());
	timer->start(1000);
}

Temperature::~Temperature()
{
	delete ui;
	timer->stop();
	delete timer;
}
void Temperature::addData(){
	qreal time=QDateTime::currentDateTime().toTime_t();
	qreal tempValue=this->getData();
	this->timeList.append(time);
	this->tempList.append(tempValue);
	if(this->timeList.count()>120){
		this->tempList.removeFirst();
		this->timeList.removeFirst();
	}

	ui->diagram->graph(0)->setData(QVector<qreal>::fromList(timeList),QVector<qreal>::fromList(tempList));//addData(time,tempValue);
	ui->diagram->xAxis->setRange(time,120, Qt::AlignRight);

	maxTemp=qMax(tempValue,maxTemp);
	minTemp=qMin(tempValue,minTemp);

	ui->diagram->yAxis->setRange(minTemp*0.9,maxTemp*1.1);

	ui->diagram->replot();

	ui->temp->display(tempValue);
}
qreal Temperature::getData(){
	QProcess process(this);
	process.setProgram("vcgencmd measure_temp");
	process.start("vcgencmd measure_temp");
	process.waitForFinished();
	QString data(process.readAll());
	process.close();
	return data.replace(QRegularExpression("temp=|'C\\n"),"").toDouble();
}
