#include "NetworkSpeed.h"
#include "ui_NetworkSpeed.h"
#include "Logger.h"
#include <QTimer>
#include <QFile>
#include <QtMath>
NetworkSpeed::NetworkSpeed(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NetworkSpeed)
{
	ui->setupUi(this);
	//流量统计置零
	uploaded=0;
	downloaded=0;
	timer=new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(calcSpeed()));
	timer->start(1000);
}

NetworkSpeed::~NetworkSpeed(){
	delete ui;
}


void NetworkSpeed::calcSpeed(){
#if _WIN32
	ui->download->setText("Windows不支持流量统计");
	ui->upload->setText("Windows不支持流量统计");
	ui->dSpeed->setText("Windows不支持流量统计");
	ui->uSpeed->setText("Windows不支持流量统计");
	timer->stop();
	return;
#endif
	QFile *fp=new QFile("/proc/net/dev");
    if(!fp->open(QIODevice::ReadOnly|QIODevice::Text)){
        Logger::error("获取网络流量失败!");
        return;
    }
    QTextStream in(fp);
    for(int i=0;i<3;i++) in.readLine();
    QString eth0=in.readLine();
	fp->close();
	delete fp;
	QStringList data=eth0.split(" ",QString::SkipEmptyParts);
	if(data.count()<10) return;
    quint64 newDown=data.at(1).toULong();
    quint64 newUp=data.at(9).toULong();
    quint32 dDown=newDown-downloaded;
    quint32 dUp=newUp-uploaded;
	ui->download->setText(bitCountToString(newDown));
	ui->upload->setText(bitCountToString(newUp));
	ui->dSpeed->setText(bitCountToString(dDown)+"/S");
	ui->uSpeed->setText(bitCountToString(dUp)+"/S");
	downloaded=newDown;
	uploaded=newUp;
}
QString NetworkSpeed::bitCountToString(quint64 sum){
	int rate=sum>0?qFloor(qLn(sum)/qLn(1024)):0;
	const QString units[]={"B","KB","MB","GB","TB","PB","EB"};
	return QString("%1%2").arg(QString::number(sum/qPow(1024,rate),'f',1),units[rate]);
}
