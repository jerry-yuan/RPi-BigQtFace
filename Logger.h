#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QListWidget>
#include <QDateTime>
#include <QQueue>
typedef struct {
    QtMsgType type;
    QString file;
    int line;
    QString function;
    QString message;
    QDateTime time;
    bool flushTag;
} Log;
class Logger : public QThread{
    Q_OBJECT
public:
    static bool verbose;
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static Logger* instance();
    Logger();
    void enqueue(Log log);
    void addToLogWidget(Log log);
    void setLogWidget(QListWidget* widget);
protected:
    void run();
private:
    static Logger* m_instance;
    QListWidget* logWidget;
    QMutex* qMutex;
    QWaitCondition* qNotEmpty;
    QQueue<Log>* logQueue;
};

#endif // LOGGER_H
