#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
namespace Ui {
	class NetworkMonitor;
	}

class NetworkMonitor : public QWidget
{
	Q_OBJECT

public:
	explicit NetworkMonitor(QWidget *parent = 0);
	~NetworkMonitor();
private slots:
	void ethCKRequestRespond();	//IP获取&内网检测		请求响应槽
	void idInfRequestRespond();//网号状态拉取			请求响应槽

	void aliECSRequestRespond();//阿里云状态转储		请求响应槽

	void dnsCKRequestRespond();	//DNS查询			请求响应槽
	void dnsSTRequestRespond();	//DNS设定			请求响应槽

	void loginPreRequestRespond();//登网号预备		请求响应槽
	void loginPgChkRequestRespond();//检查页面		请求响应槽
	void loginRequestRespond();//登网号				请求响应槽
	void vCodeRequestRespond();//验证码				请求响应槽

	void beat();				//心跳计时
	void refreshImmediately();			//立即刷新
	void updateTime();
private:
	Ui::NetworkMonitor *ui;

	int timing;

	QNetworkAccessManager* netManager;	//请求管理
	QNetworkReply* reply;		//响应管理

	QNetworkRequest vUPC;			//v.upc请求
	QNetworkRequest netIDInfo;		//获取网号状态
	QNetworkRequest aliECS;			//网络测试
	QNetworkRequest DNSReader;		//DNS查询
	QNetworkRequest DNSPoster;		//DNS设定
	QNetworkRequest pgCheck;		//登录页面检查
	QNetworkRequest netLogin;	//登录网号
	QNetworkRequest vCodeFetch;//获取验证码

	QString netLoginQS;	//登录网号必需的一个参数
	QString validCode;		//验证码
	QString currentId;		//当前网号
	QString currentName;	//当前网号所属人
    QDateTime netIdDeadline;		//当前网号过期时间

	void startCheck();			//启动检测网络流程
	void ethCKRequest();		//IP获取&内网检测		发起请求
	void idInfRequest();		//检查网号			发起请求
	void aliECSRequest();		//阿里云状态转储		发起请求
	void dnsCKRequest();		//DNS查询			发起请求
	void dnsSTRequest();		//DNS设定			发起请求
	void loginPreRequest();		//获取登录地址			发起请求
	void loginPgChkRequest();	//检查登录页面			发起请求
	void loginRequest();		//登录网号			发起请求
	void vCodeRequest();		//更新验证码			发起请求

	void checkTerminated(QString reason="");		//检查错误中断
	void checkFinished();		//检查完成

	void showEthIP();			//显示内网IP
	void setTiming(int length);	//设定计时器长度

	QString formatTime(int t);
    QString latterFlow(quint64 f);
};

#endif // NETWORKMONITOR_H
