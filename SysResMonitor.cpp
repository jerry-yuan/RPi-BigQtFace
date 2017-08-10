#include "SysResMonitor.h"
#include "ui_SysResMonitor.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
SysResMonitor::SysResMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SysResMonitor)
{
    ui->setupUi(this);
    this->cpuIdle=this->cpuUsed=0;
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(freshCpuUsage()));
}

SysResMonitor::~SysResMonitor(){
    delete ui;
}

void SysResMonitor::activate(){
    timer->start(1000);
}

void SysResMonitor::deactivate(){
    timer->stop();
}

void SysResMonitor::freshCpuUsage(){
    QFile *fp=new QFile("/proc/stat");
    if(!fp->open(QIODevice::ReadOnly)){
        ui->cpuUsage->setText("-");
        ui->cpuUsage->setStyleSheet("color:red");
        return;
    }
    QTextStream cpu(fp);
    QStringList cpuTimeData=cpu.readLine().split(" ",QString::SkipEmptyParts);
    fp->close();
    delete fp;
    quint64 cpuUsed=0,cpuIdle=0;
    for(int i=1;i<4;i++)
        cpuUsed+=cpuTimeData.at(i).toUInt();
    cpuIdle=cpuTimeData.at(4).toUInt();
    if(this->cpuIdle+this->cpuUsed>0){
        quint64 usedSlice=cpuUsed-this->cpuUsed;
        quint64 idleSlice=cpuIdle-this->cpuIdle;
        qreal cpuUsage=usedSlice;
        cpuUsage=cpuUsage/(idleSlice+usedSlice)*100;
        ui->cpuUsage->setText(QString("%1%").arg(QString::number(cpuUsage,'f',1)));
        QPalette palette=ui->cpuUsage->palette();
        QColor t=QColor::fromRgbF(qMin((qreal)50,cpuUsage)/50,(100-qMax((qreal)50,cpuUsage))/50,0);
        palette.setColor(QPalette::WindowText,t);
        ui->cpuUsage->setPalette(palette);
        ui->cpuUsage->repaint();
    }
    this->cpuUsed=cpuUsed;
    this->cpuIdle=cpuIdle;
}
