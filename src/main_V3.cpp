/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
#include <sstream>
#include <math.h>
#include <map>
#include <algorithm>
#include "reader_writer.h"
#include "robustPredicates.h"
#include "debug.h"
#include "Vertex.h"
#include <stdlib.h>
#include "CoordinateInputGUI.h"

struct Edge {
    Vertex *vmin, *vmax;
    Edge( Vertex *v1 , Vertex *v2 ){
        vmin = std::min ( v1 , v2 ) ;
        vmax = std::max ( v1 , v2 ) ;
    }
    bool operator < (const Edge &other ) const {
        if (vmin < other .vmin) return true ;
        if (vmin > other .vmin) return false ;
        if (vmax < other .vmax) return true ;
        return false ;
    }
} ;

struct Face {
	Face *F[3] ;
	Vertex *V[3] ;
	bool deleted ;
	Face ( Vertex *v0 , Vertex *v1 , Vertex *v2 ) {
	V[0] = v0 ; V[1] = v1 ; V[2] = v2 ;
	F[0] = F[1] = F[2] = NULL;
	deleted = false ;
	}
	Edge getEdge (int k) {
	return Edge (V[k] ,V[(k+1)%3]) ;
	}

    //cz
    //!!!BOUCLE infinie de LineSearch engendr�e ICI si double coordonnee identique dans le dataset!!!
	//todo: determiner si la th�orie permet d'avoir deux (ou plusieurs) fois des coordonnee identique dans le dataset
	//todo: si la theorie le permet, analyser le code et l'algorithme robustPredicates !!
	bool inCircle ( Vertex *c ){
		double test = robustPredicates::incircle ((double*) V[0], (double*) V[1], (double*) V[2], (double*) c)
		* robustPredicates::orient2d ((double*) V[0], (double*) V[1], (double*) V[2]);
		return test>0;
	}
	Vertex centroid(){
	  double x=V[0]->x+V[1]->x+V[2]->x;
	  double y=V[0]->y+V[1]->y+V[2]->y;
	  return Vertex(x/3, y/3);

	}

	std::string print() {
		return "Face: V0: " + (V[0]->print()) + "; V1: " + (V[1]->print()) +";  V2:" + (V[2]->print());
	}
} ;

//cz
bool isWithinBigSquare(std::vector<Vertex*> &biigSquare, Vertex *nv);
void renumberBigSquareIndexes(std::vector<Vertex*> &bigSquare,int startingIndex);
bool containsPoint(std::vector<Vertex*> &vertex, double newX, double newY, double newVal);
void printExecutionTimes(bool computemodeNS);
void defineBigSquare(std::vector<Vertex*> &vertex, std::vector<Vertex*> &bigSquare);
void initFace(std::vector<Vertex*> &vertex, std::vector<Face*> &face);
void printHelp(std::string pgmname);

//double centroid(Face tri);
void computeAdjacencies ( std :: vector <Face*> &cavity) ;
void delaunayCavity (Face * f , Vertex *v , std :: vector<Face*> &cavity ,
std :: vector<Edge> &bnd, std :: vector<Face*> &otherSide ) ;
Face* lineSearch (Face * f , Vertex *v ) ;
void delaunayTrgl ( std :: vector<Vertex*> &S, std :: vector<Face*> &T) ;
void swap(double x1, double x2);
void computeAdjacencies ( std :: vector <Face*> &cavity) ;
void delaunayCavity (Face * f , Vertex *v , std :: vector<Face*> &cavity ,
std :: vector<Edge> &bnd, std :: vector<Face*> &otherSide ) ;
Face* lineSearch (Face * f , Vertex *v ) ;
void delaunayTrgl ( std :: vector<Vertex*> &S, std :: vector<Face*> &T) ;
void swap(double x1, double x2);
bool compare (Vertex* v1, Vertex* v2);
void hilbertCoord ( double x , double y , double x0 , double y0 , double xRed , double yRed , double xBlue , double yBlue , int d, int bits [ ] );
void newPoint (Vertex* S, std :: vector<Face*> &T);

//to do use the header file
//bool debug = true;
//bool debugVerbose = false;
//bool displayCoordinate = false;

bool enableHilbert = false;
bool performanceNanoSecond = true; // if false then second
bool enableGUI = false;

auto start_time = std::chrono::high_resolution_clock::now();
auto end_time = std::chrono::high_resolution_clock::now();
auto start_time_bs = std::chrono::high_resolution_clock::now();
auto end_time_bs = std::chrono::high_resolution_clock::now();
auto start_time_ht = std::chrono::high_resolution_clock::now();
auto end_time_ht = std::chrono::high_resolution_clock::now();
auto start_time_dt = std::chrono::high_resolution_clock::now();
auto end_time_dt= std::chrono::high_resolution_clock::now();
auto start_time_tr = std::chrono::high_resolution_clock::now();
auto end_time_tr = std::chrono::high_resolution_clock::now();
auto start_time_nd = std::chrono::high_resolution_clock::now();
auto end_time_nd = std::chrono::high_resolution_clock::now();
auto start_time_hc = std::chrono::high_resolution_clock::now();
auto end_time_hc = std::chrono::high_resolution_clock::now();

int main(int argc, char const *argv[]){
	//cz
	start_time = std::chrono::high_resolution_clock::now();

    // Data[3*i],Data[3*i+1] will contain ith node (for all i such that 0<=i<n)
	double *Data;

	//cz
	unsigned n;
	std::string outputFile;
	switch (argc) {
		case 1: {
			std::cout << "Lecture du dataset au d�part du ficher " << "In/Exemple.in" << std::endl;
			n = read_nodes_txt("In/Exemple.in", &Data);
			outputFile = "Out/Triangle.out";
		}
		break;

		case 2: {
			std::string help;
			help = argv[1];
			if (help.compare("-help")==0) {
				printHelp(argv[0]);
				return 0;
			}
			else {
				std::cout << "Lecture du dataset au d�part du ficher " << argv[1] << std::endl;
				n = read_nodes_txt(argv[1], &Data);
				outputFile = "Out/Triangle.out";
			}
		}
		break;

		case 3: {
			std::cout << "Lecture du dataset au d�part du ficher " << argv[1] << std::endl;
			n = read_nodes_txt(argv[1], &Data);
			outputFile = argv[2];
		}
		break;

		default: {
			printHelp(argv[0]);
		    return -1;
		}
		break;
	}
    robustPredicates::exactinit();
    std::vector <Vertex*> listeV;
    
    for ( int i =0; i<n ; i ++) {
        Vertex *nVert = new Vertex (Data[3*i],Data[3*i+1]);

        if (debug & displayCoordinate){
			std::cout << "d:Coordinate ";
			std::cout << i;
			std::cout << ": (";
			std::cout << Data[3*i];
			std::cout << ",";
			std::cout << Data[3*i+1];
			std::cout << ",";
			std::cout << Data[3*i+2];
			std::cout << ")" << std::endl;
		}

        nVert->val = Data[3*i+2];
        nVert->num = i;
        listeV.push_back (nVert);
    }

    std::vector<Face*> face;

    std::vector <Vertex*> bigSquare;

    //Select console or GUI input
    std::string query = "";
    std::cout << "Voulez vous saisir les coordonn�es au travers d'une GUI ? (O/N)";
    getline(std::cin, query);
//    query = "O";
    if (query == "O") {
    	enableGUI=true;
    	if (debug) {
    		std::cout << "d:Mode capture graphique activ�" << std::endl;
    	}
    }
    else {
    	enableGUI=false;
    	if (debug) {
    		std::cout << "d:Mode entr�e console" << std::endl;
    	}
    }

    //Opt-in for Hilbert optimisation
    query = "";
    std::cout << "Voulez vous optimiser avec l'algorithme Hilbert ? (O/N)";
    getline(std::cin, query);
//    query = "O";
    if (query == "O") {
    	enableHilbert=true;
    	if (debug) {
    		std::cout << "d:Mode optimisation Hilbert: actif" << std::endl;
    	}
    }
    else {
    	enableHilbert=false;
    	if (debug) {
    		std::cout << "d:Mode optimisation Hilbert: passif" << std::endl;
    	}
    }

    //compute the bigSquare to determine current working coordinates area
  	start_time_bs = std::chrono::high_resolution_clock::now();
    defineBigSquare(listeV,bigSquare);
 	end_time_bs = std::chrono::high_resolution_clock::now();

	std::vector< std::string > input;
	//text array which old the user given coordinates
	std::string inputCoordinate;
    CoordinateInputGUI inputGUI;
 	if (enableGUI) {
    	if (debug) {
    		std::cout << "d:Lauching GUI" << std::endl;
    	}
       	//initialise the 2D caneva display with dataset from the file
    	inputGUI.init(listeV, bigSquare);
       	//get the complementary dataset in the similar format produced by the console input mode
      	inputCoordinate = inputGUI.getUserDataset();
 	}
 	else {
		std::cout << std::endl;
		std::cout << "Saisir un tableau de coordonn�es (x, y, valeur) pour compl�menter le dataset du fichier" << std::endl;
		std::cout <<"  S�parez les nombres de chaque entr�e par un espace et terminez par un retour � la ligne" << std::endl;
		std::cout <<"  Cloturez la saisie du tableau par une ligne vide" << std::endl;
		std::cout <<"==================================================" << std::endl;

		std::string line;
		while (true) {
			getline(std::cin, line);
			 if (line.empty()) {
				break;
			 }
			inputCoordinate += line + '\n';
		}
		std::cout << "==================================================" << std::endl;
 	}

 	if (debug) {
		std::cout << "d:Dataset fournit par l'utilisateur" << std::endl;
		std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
		std::cout << inputCoordinate;
		std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
 	}

	if (!inputCoordinate.empty()) {
		std::istringstream stream(inputCoordinate);
		for (int i=0; stream.good(); i++) {
			std::string tmp;
			stream >> tmp;
			input.push_back(tmp);
		}

		double xn;
		double yn;
		double valn;
		if (debug) {
			std::cout << "d:Nb de coordonn�es compl�mentaires: " << (input.size()/3) << std::endl;
		}
		for(int k = 0 ; (k+3) < input.size() ; k+=3) {
			std::istringstream(input[k]) >> xn;
			std::istringstream(input[k+1]) >> yn;
			std::istringstream(input[k+2]) >> valn;
			if (debug) {
				std::cout << "d:Nouvelle coordonn�e (" << input[k] << ", " << input[k+1] << ", " << input[k+2] << ")" << std::endl;
			}
			if (!containsPoint(listeV, xn, yn, valn)) {
				Vertex *nv = new Vertex(xn,yn);
				nv->val=valn;
				nv->num=listeV.size();
				//check if we are still within the current bigSquare area
				if (!isWithinBigSquare(bigSquare,nv)) {
					//recompute the bigSquare to determine current working coordinates area
					listeV.push_back(nv);
					if (debug) {
						std::cout << "d:Recalcul de bigSquare" << std::endl;
					}
					defineBigSquare(listeV,bigSquare);
				}
				else {
					listeV.push_back(nv);
				}
			}
		}
	}

    //cz
	//We now know the total number of vertex (dataset from file + dataset from user input)
	//from the input datasets
	//We can now renumber the 4 auto generated Vertex from the bigSquare
	renumberBigSquareIndexes(bigSquare,listeV.size());
    initFace(bigSquare, face);

    if (debugVerbose) {
    	std::cout << "dv: Impression des faces seed  (BigSquare)" << std::endl;
		for (int i=0; i<face.size();i++) {
			std::cout << "  dv: #" << i << face[i]->print() << std::endl;
		}
    }
    
    std::vector <Vertex*> hilListeV;
    if (enableHilbert) {
    	if (debug) {
    		std::cout << "d:Tri de " << listeV.size() << " Vertex avec l'algorithme d'Hilbert"  << std::endl;
     	}
     	//we sort the dataset with hilbert algorithm
      	start_time_ht = std::chrono::high_resolution_clock::now();
        double x0 = (bigSquare[0]->x+bigSquare[2]->x)/2;
        double y0 = (bigSquare[0]->y+bigSquare[2]->y)/2;
        double xRed = (abs(bigSquare[0]->x)+abs(bigSquare[2]->x))-x0;
        double xBlue = 0.0;
        double yRed = 0.0;
        double yBlue = (abs(bigSquare[0]->y)+abs(bigSquare[2]->y))-y0;
        int d =5;

        for (int i=0; i<listeV.size(); i++){
              double xh   = listeV[i]->x;
              double yh   = listeV[i]->y;
              double numh = listeV[i]->num;
              Vertex *hilV = new Vertex(xh,yh);
              hilV->num = numh;
              hilV->bits = new int[d];
              hilbertCoord(hilV->x, hilV->y, x0, y0, xRed, yRed, xBlue, yBlue, d, hilV->bits);
              hilListeV.push_back(hilV);
        }
        std::sort(hilListeV.begin(),hilListeV.end(),compare);
      	end_time_ht = std::chrono::high_resolution_clock::now();
     }

	if (debug) {
		if (enableHilbert) {
			std::cout << "d:G�n�ration des faces avec l'algorithme de Delaunay sur une seed de (" << face.size() << ") faces, sur une liste de vertex tri� avec l'algorithme d'Hilbert"  << std::endl;
		}
		else {
			std::cout << "d:G�n�ration des faces avec l'algorithme de Delaunay sur une seed de (" << face.size() << ") faces, sur une liste de vertex non tri�"  << std::endl;
		}
	}
	//we select the right vector for the rest of the code
    std::vector <Vertex*> listeVOpted;
	if (enableHilbert) {
		listeVOpted = listeV;
	}
	else {
		listeVOpted = listeV;
	}
	start_time_dt = std::chrono::high_resolution_clock::now();
	delaunayTrgl(listeVOpted, face);
	end_time_dt = std::chrono::high_resolution_clock::now();

    if (debugVerbose) {
    	std::cout << "dv:Printing Faces Delaunay" << std::endl;
		for (int i=0; i<face.size();i++) {
			std::cout << "  dv: #" << i << face[i]->print() << std::endl;
		}
    }

	//cz
	if (debug) {
		std::cout << "d:R�cup�ration des (" << face.size() << ") triangles" << std::endl;
 	}
	start_time_tr = std::chrono::high_resolution_clock::now();
    unsigned Triangles[3*face.size()];
    for (int i=0; i<face.size(); i++){
		Triangles[3*i] = face[i]->V[0]->num;
		Triangles[3*i+1]= face[i]->V[1]->num;
		Triangles[3*i+2]=  face[i]->V[2]->num;
    }
    //cz
	end_time_tr = std::chrono::high_resolution_clock::now();

	//cz
	if (debug) {
		std::cout << "d:R�cup�ration des (" << listeVOpted.size()+bigSquare.size() <<") noeuds" << std::endl;
 	}
	start_time_nd = std::chrono::high_resolution_clock::now();
	//we define the nodes with: the vertex (file dataset + user input dataset) in listeV vector + the 4 Vertex from biqSquare vector
	int nbnodes  =(listeVOpted.size()+bigSquare.size());
	double node[3*nbnodes];
	//we collect the the vertex (file dataset + user input dataset) in listeV vector
	for (int i=0; i<listeVOpted.size(); i++){
		 node[3*i] = listeVOpted[i]->x;
		 node[3*i+1]= listeVOpted[i]->y;
		 node[3*i+2]=  listeVOpted[i]->val;
	}
	//we add to the collectionthe 4 Vertex from biqSquare vector
	for (int i=0,j=listeVOpted.size(); i<4; i++, j++){
		 node[3*j]  = bigSquare[i]->x;
		 node[3*j+1]= bigSquare[i]->y;
		 node[3*j+2]= bigSquare[i]->val;
		 std::cout << "d:Ajout du vertex " << i << " de BigSquare," << bigSquare[i]->print() << std::endl;
	}
    end_time_nd = std::chrono::high_resolution_clock::now();

    if (debugVerbose) {
    	std::cout << "dv:Impression des noeuds" << std::endl;

    	for (int j=0; j<nbnodes; j++){
    		 std::cout << "  dv: Node #" << j << "(" << node[3*j] << "," << node[3*j+1] << "," << node[3*j+2] << ")" << std::endl;
    	}
    }

	if (enableHilbert) {
		outputFile += "_HilbertOptimised";
	}
    char* outputf = new char [outputFile.length()+1];
    std::strcpy(outputf, outputFile.c_str());

	std::cout << std::endl << "Sauvegarde des r�sultats dans le ficher ";
    std::cout << outputf << std::endl;

    write_gmsh_txt(outputf, node, nbnodes, Triangles,face.size());

   //Clean the memory : free object
    for (int i=0; i<listeV.size(); ++i) {
        delete listeV[i];
    }
    for (int i=0; i<hilListeV.size(); ++i) {
       delete hilListeV[i];
    }
    for (int i=0; i<face.size(); ++i) {
        delete face[i];
    }

    //cz
    end_time = std::chrono::high_resolution_clock::now();
    printExecutionTimes(performanceNanoSecond);
    return 0;
}

void printHelp(std::string pgmname) {
	std::cout << "Usages: "<<  std::endl;
	std::cout << " " << pgmname << " -help"<< std::endl;
	std::cout << "     Display this help" << std::endl;
	std::cout << " " << pgmname << " f.in" << std::endl;
	std::cout << "     Uses 'in' as input for the dataset" << std::endl;
	std::cout << " "  << pgmname << " f.in f.out" << std::endl;
	std::cout << "     Uses 'f.in' as input file for the dataset" << std::endl;
	std::cout << "     Uses 'f.out' as output file for the dataset" << std::endl;
}

void printExecutionTimes(bool computemodeNS) {
	//cz
	if (computemodeNS) {
		std::cout << std::endl;
		std::cout << "R�sultats des statistiques d'ex�cution en ns" << std::endl;
		std::cout << "--Dur�e totale d'utilisation du programme : ";
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de BigSquare : ";
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_bs - start_time_bs);
		std::cout << elapsed.count() << std::endl;

		if (enableHilbert) {
			std::cout << "---Dur�e d'ex�cution d'Hilbert : ";
			elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_ht - start_time_ht);
			std::cout << elapsed.count() << std::endl;
		}

		std::cout << "---Dur�e d'ex�cution de DelaunayTrgl : ";
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_dt - start_time_dt);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de la d�finition des triangles : ";
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_tr - start_time_tr);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de la d�finition des noeuds : ";
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_nd - start_time_nd);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de HilbertCoord : ";
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_hc - start_time_hc);
		std::cout << elapsed.count() << std::endl;
	}
	else {
		std::cout << std::endl;
		std::cout << "R�sultats des statistiques d'ex�cution en s" << std::endl;
		std::cout << "--Dur�e totale d'utilisation du programme : ";
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de BigSquare : ";
		elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_bs - start_time_bs);
		std::cout << elapsed.count() << std::endl;

		if (enableHilbert) {
			std::cout << "---Dur�e d'ex�cution d'Hilbert : ";
			elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_ht - start_time_ht);
			std::cout << elapsed.count() << std::endl;
		}

		std::cout << "---Dur�e d'ex�cution de DelaunayTrgl : ";
		elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_dt - start_time_dt);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de la d�finition des triangles : ";
		elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_tr - start_time_tr);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de la d�finition des noeuds : ";
		elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_nd - start_time_nd);
		std::cout << elapsed.count() << std::endl;

		std::cout << "---Dur�e d'ex�cution de HilbertCoord : ";
		elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time_hc - start_time_hc);
		std::cout << elapsed.count() << std::endl;
	}
}

//cz
bool isWithinBigSquare( std::vector<Vertex*> &bigSquare, Vertex *nv) {
	Vertex *nVert4 = bigSquare[3]; //Upper Left corner
	Vertex *nVert2 = bigSquare[1]; //Lower Right corner

	double maxX=nVert4->x;
	double minX=nVert2->x;
	double maxY=nVert2->y;
	double minY=nVert4->y;

	if ((nv->x < minX) or (nv->x > maxX) or (nv->y < minY) or (nv->y > maxY)) {
		if (debug) {
			std::cout <<"d:Coordonn�e en dehors du BigSquare actuel:" << std::endl;
			std::cout <<"d:  maxX:";
			std::cout << maxX;
			std::cout << std::endl;
			std::cout <<"d:  minX:";
			std::cout << minX;
			std::cout << std::endl;
			std::cout <<"d:  maxY:";
			std::cout << maxY;
			std::cout << std::endl;
			std::cout <<"d:  minY:";
			std::cout << minY;
			std::cout << std::endl;
		}
		return false;
	}
	return true;
}

void renumberBigSquareIndexes(std::vector<Vertex*> &bigSquare,int startingIndex) {
    if (debug) {
    	std::cout << "d:Renumbering BigSquare Vertex indexes from " << startingIndex << std::endl;
    }
	for (int i=0,j=startingIndex;i<4;i++,j++) {
		bigSquare[i]->num = j;
	}
}

//cz
bool containsPoint(std::vector<Vertex*> &vertex, double newX, double newY, double newVal) {
	int nPoints = vertex.size();
	for (int i=0; i<nPoints; i++) {

		if ((newX == vertex[i]->x) and (newY == vertex[i]->y) and (newVal == vertex[i]->val)) {

			 if (debug) {
				std::cout <<"d:Coordonn�e d�j� pr�sente au sein du dataset, � l'entr�e :" ;
				std::cout << vertex[i]->num;
				std::cout << std::endl;
			}
			return true;
		}
	}
	return false;
}

//cz
void defineBigSquare(std::vector<Vertex*> &vertex, std::vector<Vertex*> &bigSquare) {
	int nPoints = vertex.size();
	double maxX=vertex[0]->x;
	double minX=vertex[0]->x;
	double maxY=vertex[0]->y;
	double minY=vertex[0]->y;
	for (int i=0; i<nPoints; i++) {
		if (maxX<=vertex[i]->x) {
			maxX=vertex[i]->x;
		}
		if (minX>=vertex[i]->x) {
			minX=vertex[i]->x;
		}
		if (maxY<=vertex[i]->y) {
			maxY=vertex[i]->y;
		}
		if (minY>=vertex[i]->y) {
			minY=vertex[i]->y;
		}
	}
	Vertex *nVert1 = new Vertex (minX-0.1,minY-0.1);
	Vertex *nVert2 = new Vertex (minX-0.1,maxY+0.1);
	Vertex *nVert3 = new Vertex (maxX+0.1,maxY+0.1);
	Vertex *nVert4 = new Vertex (maxX+0.1,minY-0.1);
	nVert1->num = 1;
	nVert2->num = 2;
	nVert3->num = 3;
	nVert4->num = 4;

	if (debug) {
		std::cout << "d:BigSquare � d�finit:" << std::endl;
		std::cout << "d:  (minX: "<< minX-0.1 << ", " << "minY: " << minY-0.1 <<")" << std::endl;
		std::cout << "d:  (maxX: "<< maxX+0.1 << ", " << "maxY: " << maxY+0.1 <<")" << std::endl;
	}

	if (bigSquare.size()==4) {
		//this function all is a recompute of bigSquare
		bigSquare.clear();
	}

	bigSquare.push_back(nVert1);
	bigSquare.push_back(nVert2);
	bigSquare.push_back(nVert3);
	bigSquare.push_back(nVert4);

	if (displayCoordinate) {
		std::cout << "d:BigSquare � d�finit les 4 Vertex:" << std::endl;
		std::cout << "d:V0: " + (bigSquare[0]->print()) + "; V1: " + (bigSquare[1]->print()) +";  V2:" + (bigSquare[2]->print()) +";  V3:" + (bigSquare[3]->print());
	}
}


void initFace( std::vector<Vertex*> &bigSquare, std::vector<Face*> &face) {

	Face *nFace1 = new Face (bigSquare[0],
							 bigSquare[2],
							 bigSquare[1]);
	Face *nFace2 = new Face (bigSquare[0],
							 bigSquare[3],
							 bigSquare[2]);
	face.push_back(nFace1);
	face.push_back(nFace2);
	computeAdjacencies(face);
}

void computeAdjacencies ( std :: vector <Face*> &cavity) {
    std::map < Edge , std:: pair < int, Face* > >edgeToFace;
    for ( int iFace=0 ; iFace < cavity.size ( ) ; iFace++ ) {
        for ( int iEdge=0 ; iEdge < 3 ; iEdge++) {
            Edge edge = cavity [ iFace]->getEdge ( iEdge ) ;
            std::map < Edge , std :: pair < int , Face* > >::iterator it = edgeToFace.find(edge) ;
            if ( it == edgeToFace.end() ) {
                //edge has not yet been touched , so create an entry
                edgeToFace.insert ( std :: make_pair ( edge , std ::make_pair ( iEdge , cavity [ iFace ] ) ) ) ;
            }
            else {
                //Connect the two neighboring triangles
                cavity [ iFace]->F[ iEdge ] = it ->second.second ;
                it ->second.second->F[ it ->second.first ] = cavity [ iFace ] ;
                //Erase edge from the map
                edgeToFace.erase ( it ) ;
            }
        }
    }
}

void delaunayCavity (Face * f , Vertex *v , std :: vector<Face*> &cavity ,
std :: vector<Edge> &bnd, std :: vector<Face*> &otherSide ) {
  //std::cout << "3. size: " << 1 << '\n';//AAAAAAAAAAA
    if ( f->deleted ) return ;
    //std::cout << "3. size: " << 1 << '\n';//AAAAAAAAAAA
    f->deleted = true ; // Mark the triangle
    cavity.push_back( f ) ;
    for ( int iNeigh=0; iNeigh<3 ; iNeigh++) {
        if ( f->F[ iNeigh ] == NULL) {
            bnd.push_back ( f->getEdge ( iNeigh ) ) ;
        }
        else if ( !f->F[ iNeigh]-> inCircle (v) ) {
            bnd.push_back ( f->getEdge ( iNeigh ) ) ;
            if ( !f->F[ iNeigh]->deleted ) {
                otherSide.push_back( f->F[ iNeigh ] ) ;
                f->F[ iNeigh]->deleted = true ;
            }
        }
        else delaunayCavity ( f->F[ iNeigh ] , v , cavity ,bnd, otherSide ) ;
    }
}


Face* lineSearch (Face *f , Vertex *v ) {
/*
	if (debugVerbose) {
		std::cout << "dv:Line search" ;
		std::cout << v->print();
		std::cout << std::endl;
	}
*/
    while (1) {
        if ( f == NULL) return NULL; // we should NEVER return here
	//std::cout << "2. size: " << 1 << '\n';//AAAAAAAAAAA

        //cz
        //!!!BOUCLE infinie ICI si double coordonnee identique dans le dataset!!!
        if ( f->inCircle(v)) return f; // il faudrait 4 pts
	//std::cout << "2. size: " << 1 << '\n';//AAAAAAAAAAA
        Vertex c = f->centroid() ;
	//std::cout << "2. size: " << 1 << '\n';//AAAAAAAAAAA
        for ( int iNeigh=0; iNeigh<3 ; iNeigh++) {
            Edge e = f->getEdge ( iNeigh ) ;
            //  std::cout << "2. size: " << iNeigh << '\n';//AAAAAAAAAAA
            // (double*) pour forcer conversion type de Vertex a double
            if ( robustPredicates::orient2d ((double*) &c ,(double*)v ,(double*) e .vmin) *
                robustPredicates::orient2d ((double*) &c ,(double*) v ,(double*) e.vmax) < 0 &&
                robustPredicates::orient2d ((double*) e.vmin,(double*) e.vmax,(double*) &c ) *
                robustPredicates::orient2d ((double*)e.vmin,(double*) e.vmax,(double*) v) < 0) {
                f = f->F[ iNeigh ] ;
                break ;
            }
        }
    }

 }


void delaunayTrgl ( std :: vector<Vertex*> &S, std :: vector<Face*> &T) {

    for ( int iP=0 ; iP < S.size( ); iP++ ) {

	//std::cout << "1. size: " << T.size() << '\n';//AAAAAAAA

    	//cz
    	//calcul infini ici, si double point identique dans le dataset
    	Face *f = lineSearch ( T[0] , S[iP]) ;

        //Face *f = lineSearch ( T.back() , S[iP]) ;
    	//std::cout << "1. size: " << T.size() << '\n';//AAAAAAAA
        std :: vector<Face*> cavity;
        std :: vector<Edge> bnd;
        std :: vector<Face*> otherSide;
        delaunayCavity ( f , S[ iP ] , cavity , bnd, otherSide ) ;
        if (bnd. size ( ) != cavity.size ( ) + 2) throw;
        for ( int i =0; i<cavity.size ( ) ; i++) {

            // reuse memory s lot s of invalid elements
            cavity [ i ]->deleted = false ;
            cavity [ i ]->F[0] = cavity[ i ]->F[1] = cavity[ i ]->F[2] = NULL;
            cavity [ i ]->V[0] = bnd[ i ].vmin ;
            cavity [ i ]->V[1] = bnd[ i ].vmax ;
            cavity [ i ]->V[2] = S [iP] ;
        }

	// ATTENTION : Important
	for (int i=0; i<otherSide.size(); i++){
		otherSide[i]->deleted = false;
	      }
	
        unsigned int cSize = cavity . size ( ) ;


        for ( int i =cSize ; i <cSize +2; i ++) {
            Face *newf = new Face (bnd[ i ].vmin ,bnd[ i ].vmax , S[ iP ] ) ;
            T.push_back(newf) ;
            cavity.push_back(newf ) ;
        }

        for ( int i =0; i<otherSide.size ( ) ; i++)
            if ( otherSide [ i ] ) cavity.push_back( otherSide [ i ] ) ;


        computeAdjacencies( cavity ) ;
    }
}

void newPoint (Vertex* S, std :: vector<Face*> &T) {
	Face *f = lineSearch ( T[0] , S) ;
        //Face *f = lineSearch ( T.back() , S) ;
        std :: vector<Face*> cavity;
        std :: vector<Edge> bnd;
        std :: vector<Face*> otherSide;
        delaunayCavity ( f , S , cavity , bnd, otherSide ) ;
        if (bnd.size( ) != cavity.size( ) + 2) throw;
        for ( int i =0; i<cavity.size ( ) ; i++) {
            // reuse memory s lot s of invalid elements
            cavity [ i ]->deleted = false ;
            cavity [ i ]->F[0] = cavity[ i ]->F[1] = cavity[ i ]->F[2] = NULL;
            cavity [ i ]->V[0] = bnd[ i ].vmin ;
            cavity [ i ]->V[1] = bnd[ i ].vmax ;
            cavity [ i ]->V[2] = S ;
        }

	// ATTENTION : Important
	for (int i=0; i<otherSide.size(); i++){
		otherSide[i]->deleted = false;
	      }
	
        unsigned int cSize = cavity . size ( ) ;
        for ( int i =cSize ; i <cSize +2; i ++) {
            Face *newf = new Face (bnd[ i ].vmin ,bnd[ i ].vmax , S ) ;
            T.push_back(newf) ;
            cavity.push_back(newf ) ;
        }
        for ( int i =0; i<otherSide.size ( ) ; i++)
            if ( otherSide [ i ] ) cavity.push_back( otherSide [ i ] ) ;
        computeAdjacencies( cavity ) ;
    
    
}


void swap(double x1, double x2){
    x1 = -x1;
    x2 = -x2;
}


bool compare (Vertex* v1, Vertex* v2){
    for(int i = 0; i < sizeof(v1->bits) ; i++){
        //return (v1->bits[i] < v2->bits[i]);
        if(v1->bits[i] < v2->bits[i]){
            return true;
        }
        if(v1->bits[i] > v2->bits[i]){
            return false;
        }
    }
    //return true;
    return false;
}
void hilbertCoord ( double x , double y , double x0 , double y0 , double xRed , double yRed , double xBlue , double yBlue , int d, int bits [ ] ) {
    for ( int i = 0; i <d; i++) {
        double coordRed = ( x-x0 ) * xRed + ( y-y0 ) * yRed ;
        double coordBlue = ( x-x0 ) * xBlue + (y-y0 ) * yBlue ;
        xRed/=2; yRed/=2; xBlue /=2; yBlue /=2;
        if (coordRed <= 0 && coordBlue <= 0) { // quadrant 0
            x0 -= (xBlue+xRed) ; y0 -= (yBlue+yRed) ;
            swap( xRed , xBlue ); swap(yRed , yBlue );
            bits [ i ] = 0;
        }
        else if (coordRed <= 0 && coordBlue >= 0) { // quadrant 1
            x0 += ( xBlue-xRed) ; y0 += ( yBlue-yRed) ;
            bits [ i ] = 1;
        }
        else if (coordRed >= 0 && coordBlue >= 0) { // quadrant 2
            x0 += ( xBlue+xRed) ; y0 += ( yBlue+yRed) ;
            bits [ i ] = 2;
        }
        else if (coordRed >= 0 && coordBlue <= 0) { // quadrant 3
            x0 += (-xBlue+xRed) ; y0 += (-yBlue+yRed) ;
            swap( xRed , xBlue ); swap(yRed , yBlue ) ;
            xBlue = -xBlue ; yBlue = -yBlue ;
            xRed = -xRed ; yRed = -yRed ;
            bits [ i ] = 3;
        }

    }
}
    


    


    
