#ifndef SYSFUNCDIALOG_H
#define SYSFUNCDIALOG_H

#include <QDialog>
#include <QTimer>
namespace Ui {
	class SysFuncDialog;
	}

class SysFuncDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SysFuncDialog(QWidget *parent = 0);
	~SysFuncDialog();

protected:
	void mousePressEvent(QMouseEvent *){
		resetTimer();
	}
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
	void halt();
	void reboot();

    void flushCounter();
    void setBacklight(int rate);
	void resetTimer();
	void brightnessLoaded();
private:
    int backlight();
	Ui::SysFuncDialog *ui;
	int count;
	QTimer* timer;
	QTimer* singleShot;
};

#endif // SYSFUNCDIALOG_H
