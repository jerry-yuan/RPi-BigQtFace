#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextCodec>
#include <QDebug>

bool Logger::verbose=true;
Logger* Logger::m_instance=NULL;
Logger::Logger(){
    //初始化线程
    logWidget=NULL;
    qMutex=new QMutex();
    qNotEmpty=new QWaitCondition();
    logQueue=new QQueue<Log>();
    this->start();
}
Logger* Logger::instance(){
    static QMutex insMutex;
    if(!Logger::m_instance){
        QMutexLocker locker(&insMutex);
        if(!Logger::m_instance)
            Logger::m_instance=new Logger();
    }
    return Logger::m_instance;
}
void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    Log temp;
    temp.type=type;
    temp.file=QString(context.file);
    temp.line=context.line;
    temp.function=QString(context.function);
    temp.message=msg;
    temp.time=QDateTime::currentDateTime();
    temp.flushTag=false;

    if(Logger::verbose){
        const char* msgStr=qPrintable(msg);
        switch (type) {
            case QtDebugMsg:
                fprintf(stdout, "[DEBUG]\t[%s]%s\n",context.function, msgStr);
                break;
            case QtInfoMsg:
                fprintf(stderr, "[INFO]\t[%s]%s\n",context.function, msgStr);
                break;
            case QtWarningMsg:
                fprintf(stderr, "[WARN]\t[%s]%s\n",context.function, msgStr);
                break;
            case QtCriticalMsg:
                fprintf(stderr, "[ERROR]\t[%s]%s\n",context.function, msgStr);
                break;
            case QtFatalMsg:
                fprintf(stderr, "[FATAL]\t[%s]%s\n",context.function, msgStr);
                abort();
        }
        fflush(stdout);
        fflush(stderr);
    }
    Logger::instance()->enqueue(temp);
    Logger::instance()->addToLogWidget(temp);
}
void Logger::setLogWidget(QListWidget* widget){
    this->logWidget=widget;
    Log t;
    t.flushTag=true;
    addToLogWidget(t);
}

void Logger::addToLogWidget(Log log){
    static QQueue<Log> widgetCache;
    if(!log.flushTag)
        widgetCache.enqueue(log);
    if(this->logWidget==NULL) return;
    while(!widgetCache.empty()){
        log=widgetCache.dequeue();
        QString LogContent=log.time.toString("[hh:mm:ss]")+log.message;
        QListWidgetItem* item=new QListWidgetItem();
        item->setText(LogContent);
        switch(log.type){
            case QtDebugMsg:    item->setForeground(QBrush(Qt::black));break;
            case QtInfoMsg:     item->setForeground(QBrush(Qt::blue));break;
            case QtWarningMsg:  item->setForeground(QBrush(Qt::darkYellow));break;
            case QtCriticalMsg: item->setForeground(QBrush(Qt::red));break;
            default:break;
        }
        logWidget->addItem(item);
        logWidget->scrollToBottom();
    }
}
void Logger::enqueue(Log log){
    logQueue->enqueue(log);
    qNotEmpty->wakeAll();
}


void Logger::run(){
    QSqlDatabase db;
    QSqlQuery sqlHandle(db);
    Log temp;
    while(1){
        qMutex->lock();
        if(logQueue->length()<1)
            qNotEmpty->wait(qMutex);
        qMutex->unlock();
        db.transaction();
        //DBUtil::beginTransaction();
        //sqlHandle=DBUtil::getSqlHandle();
        while(!logQueue->empty()){
            temp=logQueue->dequeue();
            sqlHandle.prepare("insert into faceLog (timestamp,content,type,file,line,function) values (?,?,?,?,?,?)");
            sqlHandle.bindValue(0,QVariant(temp.time.toTime_t()));
            sqlHandle.bindValue(1,temp.message);
            sqlHandle.bindValue(2,QVariant(temp.type));
            sqlHandle.bindValue(3,temp.file);
            sqlHandle.bindValue(4,QVariant(temp.line));
            sqlHandle.bindValue(5,temp.function);
            sqlHandle.exec();
        }
        db.commit();
        //DBUtil::commitTransaction();
    }
}
