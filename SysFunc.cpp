#include "SysFunc.h"

SysFunc* SysFunc::instance=NULL;

SysFunc::SysFunc(QObject *parent) : QObject(parent){
	f
}

SysFunc* SysFunc::getInstance(){
	if(SysFunc::instance==NULL)
		SysFunc::instance=new SysFunc();
	return SysFunc::instance;
}


void SysFunc::halt();
void reboot();
