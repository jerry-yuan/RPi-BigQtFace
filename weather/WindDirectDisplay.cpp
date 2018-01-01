#include "WindDirectDisplay.h"
#include <QPainter>
#include <QPointF>
#include <QtMath>
WindDirectDisplay::WindDirectDisplay(QWidget *parent) : QWidget(parent)
{
	_degree=0;
}
void WindDirectDisplay::setDegree(int degree){
	_degree=degree;
	this->repaint();
}

void WindDirectDisplay::paintEvent(QPaintEvent *){
	QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	qreal w=this->width();
	qreal h=this->height();
	qreal w_2=w/2;
	qreal h_2=h/2;
	painter.translate(QPointF(w_2,h_2));
	//painter.drawLine(QPointF(-w_2*0.3,0),QPointF(w_2*0.3,0));
	//painter.drawLine(QPointF(0,-h_2*0.3),QPointF(0,h_2*0.3));
	//绘制 N E S W 方向标
	QFont font("YaHei Consolas Hybrid");
	font.setPixelSize((w+h)/2*0.1);
	QFontMetricsF fontM(font);
	painter.setFont(font);
	painter.drawText(QPointF(-w*0.4-fontM.width("W")/2,fontM.height()/4),"W");
	painter.drawText(QPointF(-fontM.width("N")/2,-h*0.4+fontM.height()/4),"N");
	painter.drawText(QPointF(w*0.4-fontM.width("E")/2,fontM.height()/4),"E");
	painter.drawText(QPointF(-fontM.width("S")/2,h*0.4+fontM.height()/4),"S");
	//绘制箭头
	painter.rotate(_degree);
	QPointF head(0,-h*0.3);
	QPointF zero(0,0);
	QPointF tailL(-w*0.25*qCos((qreal)54/180*M_PI),h*0.25*qSin((qreal)54/180*M_PI));
	QPointF tailR( w*0.25*qCos((qreal)54/180*M_PI),h*0.25*qSin((qreal)54/180*M_PI));
	QPointF points[4]={head,tailL,zero,tailR};
	painter.setBrush(Qt::blue);
	painter.drawPolygon(points,4);
}
