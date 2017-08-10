#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QListWidget>
#include <QLocalServer>
#include <QLocalSocket>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

enum LogType{Log=0,Info=1,Warning=2,Error=3};
typedef struct{
	uint timestamp;
	QString content;
	LogType type;
} log_t;
class Logger : public QThread
{
	Q_OBJECT
public:
	Logger();
	static void warning(QString content){return Logger::logout(content,Warning);}
	static void info(QString content){return Logger::logout(content,Info);}
	static void log(QString content){return Logger::logout(content,Log);}
	static void error(QString content){return Logger::logout(content,Error);}
	static void setLogWidget(QListWidget* logList);
	static void addLogToWidget(log_t log);
	static void logout(QString content,QString type);
	static void logout(QString content, LogType type=Log);
	static Logger* getInstance();
    static void clearWidget();

	void addLogToQueue(log_t log);
    void clearLogWidget();
	QQueue<log_t>* queue();
protected:
	void run();
private:

	static QListWidget* logList;
	static Logger* threadIns;
	QMutex* qMutex;
	QWaitCondition* qNotEmpty;
	QQueue<log_t>* logQueue;
};

#endif // LOGGER_H
