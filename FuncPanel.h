#ifndef FUNCPANEL
#define FUNCPANEL
#include <QtPlugin>
#include <QWidget>
class FuncPanel:public QWidget{
public:
    FuncPanel(QWidget* parent=0):QWidget(parent){}
    virtual QIcon icon()=0;
    virtual QString name()=0;
    virtual QString title()=0;
    virtual void active()=0;
    virtual void deactive()=0;
};

Q_DECLARE_INTERFACE(FuncPanel,"cn.jerryzone.BigFace.FuncPanel")
#endif // FUNCPANEL

