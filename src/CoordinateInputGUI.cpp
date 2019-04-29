/*
 * CoordinateInputGUI.cpp
 *
 *  Created on:
 *      Author: CarmeloZaccone
 */

//#include "debug.h"
#include <iostream>
#include <QGuiApplication>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPointF>
#include <QVector>
#include <string>
#include <sstream>

#include "CoordinateInputGUI.h"
#include "CustomGraphicsView.h"
#include "CustomGraphicsScene.h"

bool debug2 = true;
bool debugVerbose2 = false;

double scalingFactor;
std::vector <Vertex*> listeV;
std::vector <Vertex*> bigSquare;
std::string inputCoordinate;

CoordinateInputGUI::CoordinateInputGUI() {}
CoordinateInputGUI::~CoordinateInputGUI() {}

int CoordinateInputGUI::init(std::vector <Vertex*> &listeVinit, std::vector<Vertex*> &bigSquareInit) {
	//Graphical display of dataset and bigSquare
	listeV = listeVinit;
	bigSquare=bigSquareInit;
	inputCoordinate="";
	int panelWidth= 800;
	int panelHeight = 600;
	int radius = 4;

	if (debug2) {
		std::cout << "d:Définition de la taille de l'affichage (" << panelWidth << "X"<< panelHeight <<")"<< std::endl;
	}

	scalingFactor =1;

/*
	if (debug2) {
		addPointToDataset(2,2);
		addPointToDataset(3,3);
		addPointToDataset(1,6);
	}
*/
	int argc = 1;
	char *argv[argc] = {"InputGUI"};

	QApplication app(argc, argv);

	QVector <QPointF> points;

	// Fill in points with n number of points
	int nPoints = listeVinit.size();
	for (int i=0; i<nPoints; i++) {
		double x,y;
		x =listeVinit[i]->x;
		y =listeVinit[i]->y;

		if (debugVerbose2) {
			std::cout << "dv:Ajout du point à tracer (" << x << ","<< y<<")"<<std::endl;
		}
		points.append(QPointF(x, -y)); // y axis is going downwards in QT
	}

	// Create a view, put a scene in it and add tiny circles in the scene
	CustomGraphicsView * view = new CustomGraphicsView();
	CustomGraphicsScene * scene = new CustomGraphicsScene();

	//draw the bigSquare
	double xUpperLeftCorner,yUpperLeftCorner,  width, height;

	Vertex *nVert4 = bigSquare[3]; //Upper Left corner
	Vertex *nVert2 = bigSquare[1]; //Lower Right corner

	xUpperLeftCorner=nVert2->x;
	yUpperLeftCorner=-nVert2->y; // y axis is going downwards in QT

	width=(nVert4->x)-(nVert2->x);
	height=(nVert2->y)-(nVert4->y);

	//compute the best init zoom to display the Plot within the scene
	double tmp1, tmp2;
	tmp1 = width / panelWidth;
	tmp2 = height / panelHeight;
	if (debug2) {
		std::cout << "d:Width [" << width << "], height [" << height << "]" << std::endl;
		std::cout << "d:Calcul du scaling factor (width/panelWidth) [" << tmp1 << "], (height/panelHeight) [" << tmp2 << "]" << std::endl;
	}
	if ((width<=panelWidth) or (height<=panelHeight)) {
		//zoomin
		scalingFactor = 1/std::max(tmp1, tmp2);
	}
	else {
		//zoomout
		scalingFactor = std::max(tmp1, tmp2);
	}

	if (debug2) {
		std::cout << "d:Scaling factor [" << scalingFactor << "]" <<std::endl;
	}
//	scalingFactor = 2;

//	std::cout << "xUpperLeftCorner" << xUpperLeftCorner << std::endl;
//	std::cout << "yUpperLeftCorner" << yUpperLeftCorner << std::endl;

//	int canavasmargin=5;

	view->setScene(scene);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

//	view->setGeometry((-panelWidth),(-panelHeight),(2*panelWidth),(2*panelHeight));

	view->setSceneRect((-panelWidth),(-panelHeight),(2*panelWidth),(2*panelHeight));
	view->centerOn(0,0);

//	view->centerOn(xUpperLeftCorner+(width/2),yUpperLeftCorner-(height/2));

	//draw coordinate axis
	QPen pen;  // creates a default pen

	pen.setStyle(Qt::SolidLine);
	pen.setWidth(3);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);

	//x axis
	pen.setBrush(Qt::green);
	//x scale from ' half of the bigSquare with at minus of ZERO absis to half of the bigSquare width after the bigsquare left/right corner
	if (xUpperLeftCorner>=0) {
		scene->addLine((-(width/2))*scalingFactor,0,(xUpperLeftCorner+(1.5*width))*scalingFactor,0,pen);
	}
	else {
		if ((xUpperLeftCorner+(1.5*width))>=0) {
			scene->addLine((xUpperLeftCorner-(width/2))*scalingFactor,0,(xUpperLeftCorner+(1.5*width))*scalingFactor,0,pen);
		}
		else {
			scene->addLine((xUpperLeftCorner-(width/2))*scalingFactor,0,(width/2)*scalingFactor,0,pen);
		}
	}
	//y axis
	//y scale from ' half of the bigSquare height at minus of ZERO absis to half of the bigSquare height after the bigsquare lower/upper corner
	pen.setBrush(Qt::blue);
	if ((yUpperLeftCorner*scalingFactor)>=0) {
		scene->addLine(0,(-(height/2))*scalingFactor,0,(yUpperLeftCorner+(1.5*height))*scalingFactor,pen);
	}
	else {
		if (((xUpperLeftCorner+(1.5*width))*scalingFactor)>=0) {
			scene->addLine(0,(yUpperLeftCorner-(height/2))*scalingFactor,0,(yUpperLeftCorner+(1.5*height))*scalingFactor,pen);
		}
		else {
			scene->addLine(0,(yUpperLeftCorner-(height/2))*scalingFactor,0,(height/2)*scalingFactor,pen);
		}
	}

	//draw the bigsquare rectangle
	pen.setStyle(Qt::DashLine);
	pen.setBrush(Qt::red);
	scene->addRect((xUpperLeftCorner)*scalingFactor,(yUpperLeftCorner)*scalingFactor,(width)*scalingFactor, (height)*scalingFactor,pen);

	pen.setBrush(Qt::gray);
	//tmp set this point to have a plot at coner right of the panel in case all dot from the dataset are very small
	//todo update chck QT resising api
	scene->addEllipse(panelWidth,panelHeight,0,0,pen);

	pen.setBrush(Qt::black);
	for(int i = 0; i< points.size(); i++) {
		scene->addEllipse((points[i].x())*scalingFactor, (points[i].y())*scalingFactor, radius, radius, pen);
	}

	// Show the view
	view->show();

	return app.exec();
}

std::string CoordinateInputGUI::getUserDataset() {
	return inputCoordinate;
}

void CoordinateInputGUI::addPointToDataset(double xGUI, double yGUI) {
	double x,y;
	//adjust the scaling in case the display view was zoomed
	x = xGUI/scalingFactor;
	y = yGUI/scalingFactor;

	double valn;
	//set the valn to a random value ranging between x and y
	int max, min;
	if (x>y) {
		max=x;
		min=y;
	}
	else {
		max=y;
		min=x;
	}
	int random = ((std::rand()%(max - min +1)) + min);
	valn = random;

	if (debug2) {
		std::cout << "d:Nouvelle coordonnée (" << x << ", " << y << ", " << valn << ")" << std::endl;
	}

	std::ostringstream strs;
	strs << x;
	strs << " ";
	strs << y;
	strs << " ";
	strs << valn;
	std::string str = strs.str();

	inputCoordinate += str +'\n';
}
