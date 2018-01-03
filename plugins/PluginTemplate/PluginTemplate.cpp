#include "PluginTemplate.h"
#include "ui_PluginTemplate.h"

PluginTemplate::PluginTemplate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginTemplate)
{
    ui->setupUi(this);
}

PluginTemplate::~PluginTemplate()
{
    delete ui;
}
void PluginTemplate::active(){
    //actived
}

void PluginTemplate::deactive(){
    //deactive
}
