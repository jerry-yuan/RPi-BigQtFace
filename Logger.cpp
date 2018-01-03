#include "Logger.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QQueue>
#include <QDebug>
#include <QThreadPool>
#include <QMutex>
#include <QTimer>
#include "EventServer.h"
Logger* Logger::m_instance=0;
QListWidget* Logger::logList=0;
void Logger::setLogWidget(QListWidget *logList){
	Logger::logList=logList;
	logList->clear();
    log_t t;
    t.type=Fresh;
    Logger::addLogToWidget(t);
    //挂载清空
    EventServer::instance()->addMethod(Logger::instance(),"clearLogWidget");
}
QQueue<log_t> *Logger::queue(){
	return this->logQueue;
}

void Logger::addLogToWidget(log_t log){
    static QQueue<log_t> widgetCache;
    if(log.type!=Fresh)
        widgetCache.enqueue(log);
	if(Logger::logList==NULL) return;
    while(!widgetCache.empty()){
        log=widgetCache.dequeue();
        QString LogContent=QDateTime::fromTime_t(log.timestamp).toString("[hh:mm:ss]")+log.content;
        QListWidgetItem* item=new QListWidgetItem();
        item->setText(LogContent);
        switch(log.type){
            case Log:		item->setForeground(QBrush(Qt::black));break;
            case Info:		item->setForeground(QBrush(Qt::blue));break;
            case Warning:	item->setForeground(QBrush(Qt::darkYellow));break;
            case Error:		item->setForeground(QBrush(Qt::red));break;
            default:break;
        }
        logList->addItem(item);
        logList->scrollToBottom();
    }
}
void Logger::clearLogWidget(QVariantHash){
    logList->clear();
}


void Logger::logout(QString content, QString type){
	QHash<QString,LogType> *logTypeMatching=new QHash<QString,LogType>();
    logTypeMatching->insert("info",Info);
    logTypeMatching->insert("warning",Warning);
    logTypeMatching->insert("error",Error);
    logTypeMatching->insert("log",Log);
    if(logTypeMatching->contains(type.toLower()))
        Logger::logout(content,logTypeMatching->value(type.toLower()));
	else
		Logger::logout(content);
}

void Logger::logout(QString content, LogType type){
	log_t temp;
	temp.timestamp=QDateTime::currentDateTime().toTime_t();
	temp.content=content;
	temp.type=type;
	//写屏幕
	Logger::addLogToWidget(temp);
	//写数据库
    Logger::instance()->addLogToQueue(temp);
}
/*
 * 单例函数
 */
Logger* Logger::instance(){
    static QMutex insMutex;
    if(!Logger::m_instance){
        QMutexLocker locker(&insMutex);
        if(!Logger::m_instance)
            Logger::m_instance=new Logger();
    }
    return Logger::m_instance;
}

/*
 * 线程函数
*/
Logger::Logger(){
	//初始化线程
	qMutex=new QMutex();
	qNotEmpty=new QWaitCondition();
	logQueue=new QQueue<log_t>();
	this->start();
}
void Logger::addLogToQueue(log_t log){
	if(!logQueue) logQueue=new QQueue<log_t>();
	logQueue->enqueue(log);
	qNotEmpty->wakeAll();
}
void Logger::run(){
    QSqlDatabase db;
    QSqlQuery sqlHandle(db);
	log_t temp;
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
            sqlHandle.prepare("insert into faceLog (timestamp,content,type) values (?,?,?)");
            sqlHandle.bindValue(0,QVariant(temp.timestamp));
            sqlHandle.bindValue(1,temp.content);
            sqlHandle.bindValue(2,QVariant(temp.type));
            sqlHandle.exec();
		}
        //DBUtil::commitTransaction();
	}
}
