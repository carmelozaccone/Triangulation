/*
 * QGraphicsScene.h
 *
 *  Created on:
 *      Author: CarmeloZaccone
 */

#ifndef SRC_CUSTOMGRAPHICSSCENE_H_
#define SRC_CUSTOMGRAPHICSSCENE_H_

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QList>

class CustomGraphicsScene : public QGraphicsScene {
//    Q_OBJECT
public:
    explicit CustomGraphicsScene(QObject *parent = 0);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
signals:

public slots:

private:
    QList <QPointF> m_points;
};

#endif /* SRC_CUSTOMGRAPHICSSCENE_H_ */
