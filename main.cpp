#include "Face.h"
#include "Logger.h"
#include "WebsocketServer.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <wiringPi.h>
#include <Beeper.h>
int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    app.setApplicationDisplayName("RPi-BigFace");
    app.setApplicationName("RPi-BigFace");
    app.setApplicationVersion("v2.0");
    //Parse command line parameters and prepare the parameters
    QCommandLineParser cmdParser;
    cmdParser.addVersionOption();
    cmdParser.addHelpOption();
    QCommandLineOption verbose("verbose","Set if show all logs.");
    cmdParser.addOption(verbose);
    QCommandLineOption dbpath("dbpath","Set temporary database path.","dbpath",qApp->applicationDirPath()+"/Face.db");
    cmdParser.addOption(dbpath);
    cmdParser.process(app);
    //Prepare SQLite Database
    QSqlDatabase defaultDatabase=QSqlDatabase::addDatabase("QSQLITE");
    defaultDatabase.setDatabaseName(cmdParser.value(dbpath));
    defaultDatabase.open();
    //Install the Message Handler
    Logger::verbose=cmdParser.isSet(verbose);
    qInstallMessageHandler(Logger::messageHandler);
    qInfo()<<"程序启动正常";
    //WiringPi
    wiringPiSetup();
    //Instance EventServer
    EventServer::instance();
    //Instance WebSocket Server
    WebsocketServer::instance();
    //Instance Face
	Face w;
	w.show();
    return app.exec();
}
