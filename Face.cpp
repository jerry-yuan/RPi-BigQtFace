#include "Face.h"
#include "ui_Face.h"
#include "Logger.h"
#include "SysFuncDialog.h"
#include "GPIOAdapter.h"
#include "FuncPanel.h"
#include "HaltDialog.h"

#include <QFileInfoList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QStringList>

#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QPluginLoader>
Face::Face(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Face)
{
	ui->setupUi(this);
    this->loader=NULL;
    Logger::instance()->setLogWidget(ui->logWidget);
    HaltDialog::instance();
	connect(ui->clock,SIGNAL(timeMutated()),ui->networkMonitor,SLOT(updateTime()));

	connect(ui->container,SIGNAL(currentChanged(int)),this,SLOT(pageChanged(int)));
    connect(this,SIGNAL(pageChanged()),ui->clock,SLOT(deactivate()));
	connect(this,SIGNAL(page0Actived()),ui->clock,SLOT(activate()));
    connect(this,SIGNAL(pageChanged()),ui->weatherWidget,SLOT(deactivate()));
	connect(this,SIGNAL(page0Actived()),ui->weatherWidget,SLOT(flushAll()));
    connect(this,SIGNAL(pageChanged()),ui->sysResMonitor,SLOT(deactivate()));
    connect(this,SIGNAL(page0Actived()),ui->sysResMonitor,SLOT(activate()));
    //挂载系统操作
    connect(ui->sysFunc,SIGNAL(clicked()),this,SLOT(showSysFunc()));
	//挂载返回按钮
	connect(ui->goBackBtn,SIGNAL(clicked()),this,SLOT(goBack()));
	//初始化后激活当前页
    emit page0Actived();
}

Face::~Face(){
	delete ui;
}
void Face::pageChanged(int index){
	emit pageChanged();
	switch(index){
	case 0:
		emit page0Actived();
		break;
	case 1:
		emit page1Actived();
		break;
	default:
		qDebug()<<"WTF?"<<index;
	}
}
void Face::show(){
    QMainWindow::show();
    QTimer::singleShot(1000,this,SLOT(loadFuncBtn()));
}

void Face::loadFuncBtn(){
    QDir pluginDir=QDir(qApp->applicationDirPath());
    pluginDir.cd("plugins");
#ifdef UNIX
    pluginDir.setNameFilters(QStringList("*.so"));
#endif
#ifdef WIN32
    pluginDir.setNameFilters(QStringList("*.dll"));
#endif
    QFileInfoList list = pluginDir.entryInfoList();
    if(list.length()>0){
        int counter=1;
        ui->funcBtnLoadbar->setRange(0,list.length());

        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);

        QFont btnFont;
        btnFont.setPointSize(12);

        foreach(QFileInfo info,list){
            QPluginLoader loader(info.absoluteFilePath());
            if(loader.load()){
                QPushButton *funcBtn=new QPushButton(ui->funcBtnBar);
                funcBtn->setObjectName("funcBtn_"+info.fileName());
                FuncPanel* instance=qobject_cast<FuncPanel*>(loader.instance());

                funcBtn->setText(instance->name());
                funcBtn->setIcon(instance->icon());

                loader.unload();
                funcBtn->setFont(btnFont);
                funcBtn->setIconSize(QSize(24,24));

                sizePolicy.setHeightForWidth(funcBtn->hasHeightForWidth());
                funcBtn->setSizePolicy(sizePolicy);

                ui->funcBtnLayout->addWidget(funcBtn);

                connect(funcBtn,SIGNAL(clicked()),this,SLOT(activeFunc()));
            }
            ui->funcBtnLoadbar->setValue(counter++);
        }
    }
    ui->funcBtnLoadbar->setVisible(false);
}

void Face::activeFunc(){
	QString objName=sender()->objectName();
    QString libFile=objName.split('_').at(1);
    QDir pluginDir(qApp->applicationDirPath());
    pluginDir.cd("plugins");
    FuncPanel* newWidget=NULL;
    loader=new QPluginLoader(pluginDir.absoluteFilePath(libFile));
    if(loader->load()){
        newWidget=qobject_cast<FuncPanel*>(loader->instance());
        ui->funcTitle->setText(newWidget->title());
        ui->funcPanel->setWidget(newWidget);
        ui->container->setCurrentIndex(1);
        newWidget->active();
    }else{
        QMessageBox* box=new QMessageBox(QMessageBox::Critical,"Error","加载错误:无法加载"+libFile+"\n"+loader->errorString());
        box->setFont(QFont("YaHei Consolas Hybrid"));
        box->show();
    }
}
void Face::goBack(){
    ui->container->setCurrentIndex(0);
    FuncPanel* widget=qobject_cast<FuncPanel*>(ui->funcPanel->widget());
	ui->funcPanel->setWidget(NULL);
    widget->deactive();
    loader->unload();
    delete loader;
    loader=NULL;
}
void Face::showSysFunc(){
    SysFuncDialog dialog(this);
    dialog.exec();
}
