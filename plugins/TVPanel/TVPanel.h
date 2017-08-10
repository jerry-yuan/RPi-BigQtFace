#ifndef TVPANEL_H
#define TVPANEL_H

#include <QProcess>
#include <QWidget>
#include "../../FuncPanel.h"
#include <QIcon>
#include <QListWidgetItem>
#include <QDBusConnection>
#include <QDBusInterface>
namespace Ui {
class TVPanel;
}

class TVPanel : public FuncPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "cn.jerryzone.BigFace.FuncPanel")
    Q_INTERFACES(FuncPanel)
public:
    explicit TVPanel(QWidget *parent = 0);
    ~TVPanel();
    QString name(){return QString("IPTV");}
    QString title(){return QString("IPTV网络电视");}
    QIcon icon(){return QIcon(QPixmap(":/icon.svg"));}
    void active();
    void deactive();
signals:
    void dbusConnected(bool);
private slots:
    void changeChanel();
    void startOmxplayer();
    void stopOmxplayer();
    void initDBus();
private:
    QTimer* singleTimer;
    QDBusInterface* omxInterface;
    QString currentChanel;
    QHash<QString,QString>* chanels;
    QProcess* omxProcess;
    Ui::TVPanel *ui;
};

#endif // TVPANEL_H
