#ifndef SYSRESMONITOR_H
#define SYSRESMONITOR_H

#include <QWidget>
#include <QJsonValue>
namespace Ui {
class SysResMonitor;
}

class SysResMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit SysResMonitor(QWidget *parent = 0);
    ~SysResMonitor();
    Q_INVOKABLE QJsonValue getCoreInfo();
    Q_INVOKABLE QJsonValue getSysRes();
    Q_INVOKABLE QJsonValue getStorage();
public slots:
    void activate();
    void deactivate();
    void showWSClientSum(int sum);
private slots:
    void freshCpuUsage();
private:
    quint64 cpuUsed;
    quint64 cpuIdle;
    qreal cpuUsage;
    QTimer* timer;
    Ui::SysResMonitor *ui;
};

#endif // SYSRESMONITOR_H
