#include "SysResMonitor.h"
#include "ui_SysResMonitor.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QJsonObject>
#include <QJsonArray>
#include "WebsocketServer.h"
#include <stdlib.h>
SysResMonitor::SysResMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SysResMonitor)
{
    ui->setupUi(this);
    this->cpuIdle=this->cpuUsed=0;
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(freshCpuUsage()));
    connect(WebsocketServer::instance(),SIGNAL(activeClientsChanged(int)),this,SLOT(showWSClientSum(int)));

    WebsocketServer::instance()->registerObject("sysInfo",this);
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
        this->cpuUsage=cpuUsage;
    }
    this->cpuUsed=cpuUsed;
    this->cpuIdle=cpuIdle;

}
void SysResMonitor::showWSClientSum(int sum){
    ui->WS->setText(QString::number(sum));
}

QJsonValue SysResMonitor::getCoreInfo(){
    QJsonObject data;
    QProcess* proc=NULL;
    QString str;
    QFile *fp;
    //hostname
    proc=new QProcess(this);
    proc->start("hostname -f");
    proc->waitForFinished();
    data.insert("hostname",QString(proc->readAll()).replace("\n",""));
    delete proc;
    //distribution
    fp=new QFile("/etc/os-release");
    fp->open(QIODevice::ReadOnly);
    while(!fp->atEnd()){
        str=fp->readLine();
        if(str.indexOf("PRETTY_NAME")>=0)
            break;
    }
    fp->close();
    str=str.replace(QRegExp("((PRETTY_NAME=)?\")|\n"),"");
    data.insert("distribution",str);
    delete fp;
    //kernel
    proc=new QProcess(this);
    proc->start("uname -mrs");
    proc->waitForFinished();
    data.insert("kernel",QString(proc->readAll()).replace("\n",""));
    delete proc;
    //firmware
    proc=new QProcess(this);
    proc->start("uname -v");
    proc->waitForFinished();
    data.insert("firmware",QString(proc->readAll()).replace("\n",""));
    delete proc;
    //uptime
    fp=new QFile("/proc/uptime");
    fp->open(QIODevice::ReadOnly);
    str=QString(fp->readAll());
    fp->close();
    data.insert("uptime",QString(str.split(" ")[0]).toDouble());
    delete fp;
    //return
    return data;
}
QJsonValue SysResMonitor::getSysRes(){
    QJsonObject retn;
    QProcess* proc;
    QFile* fp;
    QString str;
    //CPU
    QJsonObject cpuData;
    //CPU使用率
    cpuData.insert("rates",(int)this->cpuUsage);
    //CPU温度
    fp=new QFile("/sys/class/thermal/thermal_zone0/temp");
    fp->open(QIODevice::ReadOnly);
    cpuData.insert("temp",QString(fp->readAll()).toInt()/1000);
    fp->close();
    delete fp;
    //CPU负载
    double load[3];
    QJsonObject cpuLoad;
#ifndef _WIN32
    getloadavg(load,3);
#else
    load[0]=load[1]=load[2]=0;
#endif
    cpuLoad.insert("load1",load[0]);
    cpuLoad.insert("load5",load[1]);
    cpuLoad.insert("load15",load[2]);
    cpuData.insert("load",cpuLoad);
    //CPU频率
    QJsonObject cpuRates;
    //当前频率
    fp=new QFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    fp->open(QIODevice::ReadOnly);
    str=QString(fp->readAll());
    fp->close();
    cpuRates.insert("current",QString(str.split(" ")[0]).toDouble());
    delete fp;
    //最高频率
    fp=new QFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
    fp->open(QIODevice::ReadOnly);
    str=QString(fp->readAll());
    fp->close();
    cpuRates.insert("max",QString(str.split(" ")[0]).toDouble());
    delete fp;
    //最低频率
    fp=new QFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq");
    fp->open(QIODevice::ReadOnly);
    str=QString(fp->readAll());
    fp->close();
    cpuRates.insert("min",QString(str.split(" ")[0]).toDouble());
    delete fp;
    cpuData.insert("cpuRates",cpuRates);
    retn.insert("cpu",cpuData);
    //内存
    QJsonObject memData;
    //物理内存
    QJsonObject mem,swap;
    proc=new QProcess(this);
    proc->start("free -o");
    proc->waitForFinished();
    proc->readLine();
    str=QString(proc->readLine());
    QStringList params=str.split(" ",QString::SkipEmptyParts);
    qint32 memTotal     =params[1].toInt(),
            memUsed     =params[2].toInt(),
            memFree     =params[3].toInt(),
            memBuffers  =params[4].toInt(),
            memCached   =params[6].toInt();
    mem.insert("total",memTotal);
    mem.insert("used",memUsed-memBuffers-memCached);
    mem.insert("free",memFree+memBuffers+memCached);
    memData.insert("ram",mem);
    //Swap
    str=QString(proc->readLine());
    params=str.split(" ",QString::SkipEmptyParts);
    swap.insert("total",params[1].toInt());
    swap.insert("used",params[2].toInt());
    swap.insert("free",params[3].toInt());
    memData.insert("swap",swap);
    proc->close();
    delete proc;
    retn.insert("mem",memData);
    //return
    return retn;
}
QJsonValue SysResMonitor::getStorage(){
    QJsonArray retn;
    QProcess* proc;
    QString str;
    //存储
    proc=new QProcess(this);
    proc->start("df -T");
    proc->waitForFinished();
    proc->readLine();
    while(!proc->atEnd()){
        QJsonObject storage;
        str=QString(proc->readLine());
        if(str.indexOf(QRegExp("tmpfs|rootfs|Filesystem"))>=0) continue;
        QStringList params=str.split(" ",QString::SkipEmptyParts);
        storage.insert("device",params[0]);
        storage.insert("format",params[1]);
        QJsonObject space;
        space.insert("total",params[2].toInt());
        space.insert("used",params[3].toInt());
        space.insert("free",params[4].toInt());

        storage.insert("space",space);
        storage.insert("mount",params[6]);
        retn.append(storage);
    }
    return retn;
}
