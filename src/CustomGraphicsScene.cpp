/*
 * CustomGraphicsScene.h
 *
 *  Created on:
 *      Author: CarmeloZaccone
 */
#include <iostream>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>

#include "CustomGraphicsScene.h"
#include "CoordinateInputGUI.h"

bool debug3 = false;
bool debugVerbose3 = false;

CoordinateInputGUI inputGUI;

CustomGraphicsScene::CustomGraphicsScene(QObject *parent) :  QGraphicsScene(parent) {
    this->setBackgroundBrush(Qt::gray);
}

void CustomGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void CustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);
}


void CustomGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * me)
{
    int radius = 3;
    double xn= me->scenePos().x();
    double yn= me->scenePos().y();

    QGraphicsEllipseItem * ellipse = this->addEllipse(xn - radius, yn - radius, radius*2, radius*2);

	if (debug3) {
		std::cout << "d:Détection d'une nouvelle coordonnée (" << xn << ", "<< yn <<")" <<std::endl;
	}

    ellipse->setBrush(Qt::white);
    m_points.append(me->scenePos());

    //informing the InputGui of a new user entry
    inputGUI.addPointToDataset(xn,yn);

    QGraphicsScene::mouseReleaseEvent(me);
}

void setInputGUI (CoordinateInputGUI &inputGUItmp) {
	inputGUI = inputGUItmp;
}
