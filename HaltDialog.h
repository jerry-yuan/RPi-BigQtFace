#ifndef HALTDIALOG_H
#define HALTDIALOG_H

#include <QDialog>
#include <QVariantHash>
#include <QDateTime>
#include "Beeper.h"
namespace Ui {
	class HaltDialog;
	}

class HaltDialog : public QDialog{
	Q_OBJECT
public:
	explicit HaltDialog(QWidget *parent = 0);
	~HaltDialog();
    static HaltDialog* instance();
public slots:
	void halt(int delay=60,bool beep=false);
    void reboot(int delay=60,bool beep=false);
    void triggerHalt(QVariantHash params);
    void triggerReboot(QVariantHash params);
private slots:
	void flushTitle();
	void doAction();
private:
    static HaltDialog* m_instance;
	bool beep;
	QString action;
    QDateTime triggerTime;
	int delay;
	QHash<QString,QString> map;
	QTimer* timer;
	Ui::HaltDialog *ui;
    Beeper* beeper;
};

#endif // HALTDIALOG_H
