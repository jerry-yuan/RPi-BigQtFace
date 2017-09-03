#include "TVPanel.h"
#include "ui_TVPanel.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QListWidgetItem>
#include <QDebug>
#include <QTimer>
TVPanel::TVPanel(QWidget *parent) :
    FuncPanel(parent),
    ui(new Ui::TVPanel)
{
    ui->setupUi(this);
    omxProcess=NULL;
    omxInterface=NULL;
    singleTimer=new QTimer(this);
    singleTimer->setSingleShot(true);
    singleTimer->setInterval(2000);
    QFile *fp=new QFile(":/chanels.json");
    fp->open(QIODevice::ReadOnly);
    QJsonArray chanels=QJsonDocument::fromJson(fp->readAll()).array();
    fp->close();
    delete fp;
    this->chanels=new QHash<QString,QString>();
    foreach(QJsonValue v,chanels){
        QJsonObject chanel=v.toObject();
        QString title=chanel.value("title").toString();
        QString url=chanel.value("url").toString();
        this->chanels->insert(title,url);
        ui->chanelList->addItem(new QListWidgetItem(icon(),title));
    }
    ui->chanelList->unsetCursor();
    connect(ui->chanelList,SIGNAL(clicked(QModelIndex)),this,SLOT(changeChanel()));
    connect(this,SIGNAL(dbusConnected(bool)),ui->chanelList,SLOT(setEnabled(bool)));

    connect(singleTimer,SIGNAL(timeout()),this,SLOT(initDBus()));
}

TVPanel::~TVPanel(){
    //stopOmxplayer();
    delete ui;
}

void TVPanel::active(){

}

void TVPanel::deactive(){
    ui->video->setText("正在停止...");
    stopOmxplayer();
    //qDebug()<<"deactived!";
}
void TVPanel::changeChanel(){
    QString chanel=ui->chanelList->currentItem()->data(0).toString();
    if(currentChanel==chanel) return;
    //qDebug()<<"SetEnableFalse";
    ui->chanelList->setEnabled(false);
    ui->chanelList->repaint();
    ui->video->setText(chanel);
    currentChanel=chanel;
    stopOmxplayer();
    startOmxplayer();
}
void TVPanel::startOmxplayer(){
    QPoint leftTop=ui->video->mapToGlobal(QPoint(0,0));
    int x1=leftTop.x();//9
    int y1=leftTop.y();//61
    int x2=x1+ui->video->width();
    int y2=y1+ui->video->height();
    //qDebug()<<"startOmxPlayer";
    QString command=QString("omxplayer --layer 2 --win %1 -o local %2");
    QString pos=QString("%1,%2,%3,%4")
            .arg(QString::number(x1),QString::number(y1),QString::number(x2),QString::number(y2));
    command=command.arg(pos,this->chanels->value(currentChanel));
    omxProcess=new QProcess(this);
    connect(omxProcess,SIGNAL(finished(int)),this,SLOT(startOmxplayer()));
    connect(omxProcess,SIGNAL(finished(int)),singleTimer,SLOT(stop()));
    ui->video->setText("启动播放器...");
    omxProcess->start(command);
    omxProcess->waitForStarted();
    ui->video->setText("播放器已启动.");
    singleTimer->start(2000);
}
void TVPanel::stopOmxplayer(){
    if(omxProcess!=NULL){
        disconnect(omxProcess,SIGNAL(finished(int)),this,SLOT(startOmxplayer()));
        disconnect(omxProcess,SIGNAL(finished(int)),singleTimer,SLOT(start()));
    }
    if(omxInterface!=NULL&&omxInterface->isValid())
        omxInterface->call("Stop");
    if(omxProcess!=NULL){
        omxProcess->terminate();
        if(!omxProcess->waitForFinished(1000))
            QProcess::execute("sudo killall -s SIGKILL omxplayer.bin");
    }
}
void TVPanel::initDBus(){
    singleTimer->setInterval(2000);
    if(omxProcess->state()==QProcess::NotRunning) {
        //qDebug()<<"Process Not Running!";
        ui->chanelList->setEnabled(true);
        return;
    }
    ui->video->setText("初始化控制DBUS...");
    QFile *fp=new QFile(QString("/tmp/omxplayerdbus.%1").arg(QProcessEnvironment::systemEnvironment().value("USER")));
    if(!fp->open(QIODevice::ReadOnly)){
        //qDebug()<<"Read Bus Path Failed:"<<fp->errorString()<<fp->fileName();
        delete fp;
        singleTimer->start(200);
        return;
    }
    QString busPath=fp->readAll().replace("\n","");
    fp->close();
    delete fp;
    QDBusConnection omxConnection=QDBusConnection::connectToBus(busPath,"org.mpris.MediaPlayer2.omxplayer");
    if(!omxConnection.isConnected()){
        //qDebug()<<"Establish DBus Connection Failed:"<<omxConnection.lastError();
        singleTimer->start(200);
        return;
    }
    if(omxInterface==NULL||!omxInterface->isValid()){
        if(omxInterface)
            delete omxInterface;
        omxInterface=new QDBusInterface("org.mpris.MediaPlayer2.omxplayer","/org/mpris/MediaPlayer2","org.mpris.MediaPlayer2.Player",omxConnection);
        if(!omxInterface->isValid()){
            //qDebug()<<"Instance Remote Object Failed:"<<omxInterface->lastError();
            singleTimer->start(200);
            return;
        }
    }
    ui->video->setText("控制连接建立.");
    emit dbusConnected(true);
}
