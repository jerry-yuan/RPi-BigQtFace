#include "Face.h"
#include "ui_Face.h"
#include "Logger.h"
#include "SysFuncDialog.h"
#include "Temperature.h"
#include "GPIOAdapter.h"
#include <QDebug>
#include <QString>
#include <QStringList>

#include <QJsonArray>
#include <QJsonObject>

Face::Face(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Face)
{
	ui->setupUi(this);
	Logger::setLogWidget(ui->logWidget);
	connect(ui->clock,SIGNAL(timeMutated()),ui->networkMonitor,SLOT(updateTime()));

	connect(ui->container,SIGNAL(currentChanged(int)),this,SLOT(pageChanged(int)));
	connect(this,SIGNAL(page0Actived()),ui->clock,SLOT(activate()));
	connect(this,SIGNAL(page0Actived()),ui->weatherWidget,SLOT(flushAll()));
	//ui->clock->activate();
	//ui->weatherWidget->flushAll();
	//挂载各种侧栏按钮
	QRegExp funcBtnMathch("funcBtn_\\d+");
	foreach(const QObject* obj,ui->funcBtnBar->children())
		if(funcBtnMathch.exactMatch(obj->objectName()))
			connect(static_cast<const QPushButton*>(obj),SIGNAL(clicked()),this,SLOT(activeFunc()));
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
void Face::activeFunc(){
	QString objName=sender()->objectName();
	int id=objName.split('_').at(1).toInt();
	QWidget* newWidget=NULL;
	QString title=QString();
	switch(id){
	case 0:
		newWidget=new Temperature(ui->funcPanel);
		title="BCM2835核心温度曲线";
		//ui->funcPanel->setWidget(newClock);
		break;
	}
	ui->funcTitle->setText(title);
	ui->funcPanel->setWidget(newWidget);
	ui->container->setCurrentIndex(1);
}
void Face::goBack(){
	QWidget* widget=ui->funcPanel->widget();
	ui->funcPanel->setWidget(NULL);
	delete widget;
	ui->container->setCurrentIndex(0);
}
void Face::showSysFunc(){
	SysFuncDialog dialog;
	dialog.exec();
}
