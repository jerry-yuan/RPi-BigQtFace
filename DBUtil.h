#ifndef DBUTIL_H
#define DBUTIL_H
#include <QSqlDatabase>
#include <QCoreApplication>
class DBUtil
{

private:
	DBUtil(QString dbName);
	static DBUtil *instance;
	QSqlDatabase db;
public:
	static DBUtil* getInstance();
	static QSqlQuery* getSqlHandle();
	static bool beginTransaction();
	static bool commitTransaction();
	QSqlDatabase getDatabase(){return db;}
};

#endif // DBUTIL_H
