#ifndef BEEPERTHREAD_H
#define BEEPERTHREAD_H

#include <QObject>
#include <QThread>
class BeeperThread : public QThread{
    Q_OBJECT
public:
    BeeperThread(QObject* parent=NULL);
    ~BeeperThread();
    void setDelay(quint32 delay);
    void setInterval(quint32 interval);
    void beep(quint32 times);
protected:
    void run();
private:
    static quint8 pin;
    static bool pinInited;
    quint32 delay;
    quint32 interval;
    quint32 times;

};

#endif // BEEPERTHREAD_H
