#ifndef BEEPER_H
#define BEEPER_H

#include <QObject>
#include <QTimer>
class Beeper : public QObject
{
    Q_OBJECT
public:
    explicit Beeper(QObject *parent = 0,int times=1,int delay=100,int interval=100);
    ~Beeper();
    void beep(int times=-1);
    void shutup();
signals:

public slots:
    void beepUp();
    void beepShut();
private:
    static const int PIN;
    static bool pinInited;
    QTimer* intervalTimer;
    QTimer* delayTimer;
    int _times;
    int times;
};

#endif // BEEPER_H
