#ifndef SOLVER_H
#define SOLVER_H
#include <string>
#include <unordered_map>
#include <vector>
#include "Module.h"
#include "Connection.h"
#include "B_tree.h"
#include <fstream>
#include <iostream>
#include <cmath>



using namespace std;

class Solver
{
public:
    Solver(){}
    ~Solver(){}
    
    void            readFile(const char*);
    Width_Height    calculate_w_l(int area);            // calculate feasible width for area
    void            floorplan(B_Tree t);                // calculate floorplan with given B_Tree and Modules, change each Module's location property
    void            placeBlock(Node* node, int type);   // floorplan(B_Tree t) will recursively call placeBlock with preorder, type: 0->root, 1->left, 2->right
    int             calculate_totalcost();              // calculate floorplan's total cost     
    int             findYandUpdateContour_H(int index, int from_x, int to_x);          // return Y coordinate for block and maintain vector Contour_H, from_x~to_x is the x range this block is going to placed

    int                         chip_width;     // input info
    int                         chip_height;    // input info
    unordered_map<string, int>  U_Name_Index;   // for all modules
    vector<Module*>             Modules;        // input info, for both soft and fixed modules
    vector<Node*>               Nodes;          // store every Node*'s info, for search convenience
    vector<Connection>          Connections;    // input info
    vector<Contour_horizontal>  Contour_H;      // maintain contour to insert block efficiently
    // vector<Contour_vertical>    Contour_V;      // maintain contour to insert block efficiently
    
    
    // DEBUG FUNCTIONS // for w and l given.
    void readFile_givenWL(const char*);         // ONLY FOR DEBUGGING, GIVEN SOFT BLOCKS' WIDTH AND HEIGHT
    void printModules();
    void printLocations();
    void outputFloorPlan();
};

#endif