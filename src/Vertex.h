/*
 * Vertex.h
 *
 *  Created on:
 *      Author: CarmeloZaccone
 */
#ifndef VERTEX_H
#define VERTEX_H
#include <iostream>
#include <sstream>
#include <string>

struct Vertex {
    double x , y;
    double val ;
    int num;
    int *bits;
    Vertex (double X, double Y) :
    x (X) , y(Y) { }

    std::string print () {
		std::stringstream sx,sy,sval,snum;
		std::string xstr, ystr,valstr,numstr;
		sx << x;
		xstr = sx.str();
		sy << y;
		ystr = sy.str();
		sval << val;
		valstr = sval.str();
		snum << num;
		numstr = snum.str();
		return "Vertex:  #" + numstr + "(" + xstr + "," + ystr + ";" + valstr + ")";
    }
} ;

#endif
