#ifndef FACE_H
#define FACE_H

#include <QMainWindow>
#include <QPluginLoader>
#include "EventServer.h"
namespace Ui {
	class Face;
}

class Face : public QMainWindow{
	Q_OBJECT

public:
	explicit Face(QWidget *parent = 0);
    ~Face();
signals:
	void pageChanged();
	void page0Actived();
    void page1Actived();
public slots:
    void show();
private slots:
	void pageChanged(int index);
    void loadFuncBtn();
	void activeFunc();
    void goBack();
    void showSysFunc();
private:
    QPluginLoader* loader;
	Ui::Face *ui;
};

#endif // FACE_H
