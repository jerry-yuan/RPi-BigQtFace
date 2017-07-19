#ifndef WINDDIRECTDISPLAY_H
#define WINDDIRECTDISPLAY_H

#include <QWidget>

class WindDirectDisplay : public QWidget
{
	Q_OBJECT
public:
	explicit WindDirectDisplay(QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *);
signals:

public slots:
	void setDegree(int degree);
private:
	int _degree;
};

#endif // WINDDIRECTDISPLAY_H
