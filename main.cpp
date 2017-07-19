#include "Face.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Face w;
	w.show();
	return a.exec();
}
