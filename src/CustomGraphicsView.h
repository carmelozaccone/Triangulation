/*
 * CustomGraphicsView.h
 *
 *  Created on:
 *      Author: CarmeloZaccone
 */

#ifndef SRC_CUSTOMGRAPHICSVIEW_H_
#define SRC_CUSTOMGRAPHICSVIEW_H_

#include <QGraphicsView>
#include <QtWidgets>
#include <QWheelEvent>

class CustomGraphicsView : public QGraphicsView {

	//cz
	//the MACRO is required to overload the 'slots'
	//othewise the scrolling (zoom in/out) does not work
	//However, I'm still unable to compile it in Eclipse :(
	//http://doc.qt.io/qt-5/why-moc.html
	//Q_OBJECT
public:
	explicit CustomGraphicsView(QWidget* parent = 0);
    virtual void wheelEvent(QWheelEvent *event);

signals:

public slots:
	void scalingTime(qreal);
	void animFinished();

public:
	int _numScheduledScalings ;
	int vertical_scroll_size;
};

#endif /* SRC_CUSTOMGRAPHICSVIEW_H_ */
