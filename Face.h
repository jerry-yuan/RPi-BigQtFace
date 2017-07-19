#ifndef FACE_H
#define FACE_H

#include <QMainWindow>
#include "EventServer.h"
namespace Ui {
	class Face;
	}

class Face : public QMainWindow
{
	Q_OBJECT

public:
	explicit Face(QWidget *parent = 0);
	~Face();
signals:
	void pageChanged();
	void page0Actived();
	void page1Actived();
private slots:
	void pageChanged(int index);
	void activeFunc();
	void goBack();
	void showSysFunc();
private:
	EventServer eventServer;
	Ui::Face *ui;
};

#endif // FACE_H
