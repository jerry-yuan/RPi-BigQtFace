#ifndef SYSRESMONITOR_H
#define SYSRESMONITOR_H

#include <QWidget>

namespace Ui {
class SysResMonitor;
}

class SysResMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit SysResMonitor(QWidget *parent = 0);
    ~SysResMonitor();
public slots:
    void activate();
    void deactivate();
private slots:
    void freshCpuUsage();
private:
    quint64 cpuUsed;
    quint64 cpuIdle;
    QTimer* timer;
    Ui::SysResMonitor *ui;
};

#endif // SYSRESMONITOR_H
