#ifndef LCPANEL_H
#define LCPANEL_H

#include <QWidget>
#include <../../FuncPanel.h>
#include <QIcon>
namespace Ui {
class LCPanel;
}

class LCPanel : public FuncPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "cn.jerryzone.BigFace.FuncPanel")
    Q_INTERFACES(FuncPanel)
public:
    explicit LCPanel(QWidget *parent = 0);
    ~LCPanel();
    QIcon icon(){return QIcon(":/Calendar.png");}
    QString name(){return QString("日历\n农历");}
    QString title(){return QString("日历");}
    void active();
    void deactive();
private:
    Ui::LCPanel *ui;
};

#endif // LCPANEL_H
