#ifndef PLUGINTEMPLATE_H
#define PLUGINTEMPLATE_H

#include <QWidget>
#include <../../FuncPanel.h>
namespace Ui {
class PluginTemplate;
}

class PluginTemplate : public FuncPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "cn.jerryzone.BigFace.FuncPanel")
    Q_INTERFACES(FuncPanel)
public:
    explicit PluginTemplate(QWidget *parent = 0);
    ~PluginTemplate();
    QIcon icon(){}
    QString name(){return "PluginTemplate";}
    QString title(){return "PluginTemplate Title";}
    void active();
    void deactive();
private:
    Ui::PluginTemplate *ui;
};

#endif // PLUGINTEMPLATE_H
