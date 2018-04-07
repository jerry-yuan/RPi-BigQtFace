#include "Face.h"
#include "Logger.h"
#include "GPIOAdapter.h"
#include "WebsocketServer.h"
#include <QApplication>
#include <QSqlDatabase>
int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    //Prepare SQLite Database
    QSqlDatabase defaultDatabase=QSqlDatabase::addDatabase("QSQLITE");
    defaultDatabase.setDatabaseName(qApp->applicationDirPath()+"/Face.db");
    defaultDatabase.open();
    Logger::info("程序启动正常");
    //Instance EventServer
    EventServer::instance();
    //Instance WebSocket Server
    WebsocketServer::instance();
    //Instance GPIOAdapter
    GPIOAdapter::instance();
    //Instance Face
	Face w;
	w.show();
	return a.exec();
}
