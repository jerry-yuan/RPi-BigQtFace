#include "TempPanel.h"
#include "ui_TempPanel.h"

#include <QDebug>

TempPanel::TempPanel(QWidget *parent) :
    FuncPanel(parent),
    ui(new Ui::TempPanel)
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
}

TempPanel::~TempPanel()
{
    delete ui;
    delete timer;
}
void TempPanel::addData(){
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
qreal TempPanel::getData(){
    QProcess process(this);
    process.setProgram("vcgencmd measure_temp");
    process.start("vcgencmd measure_temp");
    process.waitForFinished();
    QString data(process.readAll());
    process.close();
    return data.replace(QRegularExpression("temp=|'C\\n"),"").toDouble();
}

QIcon TempPanel::icon(){
    return QIcon(":/Dashboard.png");
}

QString TempPanel::name(){
    return QString("核心\n温度");
}
QString TempPanel::title(){
    return QString("BCM2835核心温度曲线");
}

void TempPanel::active(){
    timer->start(1000);
}

void TempPanel::deactive(){
    timer->stop();
}
