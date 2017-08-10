#include "DBUtil.h"
#include <QSqlDatabase>
#include <QSqlQuery>
DBUtil* DBUtil::instance=0;
DBUtil::DBUtil(QString dbName)
{
	db=QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbName);
	db.open();
}

DBUtil* DBUtil::getInstance(){
	if(DBUtil::instance==NULL)
        DBUtil::instance=new DBUtil(qApp->applicationDirPath()+"/Face.db");
	return DBUtil::instance;
}

bool DBUtil::beginTransaction(){
	return DBUtil::getInstance()->getDatabase().transaction();
}

bool DBUtil::commitTransaction(){
	return DBUtil::getInstance()->getDatabase().commit();
}

QSqlQuery* DBUtil::getSqlHandle(){
	return new QSqlQuery(DBUtil::getInstance()->getDatabase());
}
