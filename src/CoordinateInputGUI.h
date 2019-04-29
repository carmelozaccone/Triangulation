/*
 * CoordinateInputGUI.h
 *
 *  Created on:
 *      Author: CarmeloZaccone
 */

#ifndef SRC_COORDINATEINPUTGUI_H_
#define SRC_COORDINATEINPUTGUI_H_

#include <vector>
#include "Vertex.h"


class CoordinateInputGUI {
// Q_OBJECT
 public:
    explicit CoordinateInputGUI();
	virtual ~CoordinateInputGUI();
	int init(std::vector <Vertex*> &listeVinit, std::vector<Vertex*> &bigSquareInit);
	void addPointToDataset(double x, double y);
	std::string getUserDataset();
};

#endif /* SRC_COORDINATEINPUTGUI_H_ */
