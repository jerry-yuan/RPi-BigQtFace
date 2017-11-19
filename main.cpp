#include "Face.h"
#include <QApplication>
#include <QSqlDatabase>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    //Prepare SQLite Database
    QSqlDatabase defaultDatabase=QSqlDatabase::addDatabase("QSQLITE");
    defaultDatabase.setDatabaseName(qApp->applicationDirPath()+"/Face.db");
    defaultDatabase.open();
    //Instance Face
	Face w;
	w.show();
	return a.exec();
}
