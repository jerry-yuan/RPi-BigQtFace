#ifndef TEMPPANEL_H
#define TEMPPANEL_H

#include <QWidget>
#include <QTimer>
#include "../../FuncPanel.h"
namespace Ui {
class TempPanel;
}

class TempPanel : /*public QWidget ,*/ public FuncPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "cn.jerryzone.BigFace.FuncPanel")
    Q_INTERFACES(FuncPanel)
public:
    explicit TempPanel(QWidget *parent = 0);
    ~TempPanel();

    QIcon icon();
    QString name();
    QString title();
    void active();
    void deactive();
private slots:
    void addData();
private:
    qreal getData();

    QTimer* timer;
    qreal maxTemp,minTemp;
    QList<qreal> tempList;
    QList<qreal> timeList;
    Ui::TempPanel *ui;
};

#endif // TEMPPANEL_H
