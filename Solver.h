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



using namespace std;

class Solver
{
public:
    Solver(){}
    ~Solver(){}
    
    void readFile(const char*);
    int                         chip_width;     // input info
    int                         chip_height;    // input info
    unordered_map<string, int>  U_Name_Index;   // for all modules
    vector<Module*>             Modules;        // input info, for both soft and fixed modules
    vector<Node*>               Nodes;          // store every Node*'s info, for search convenience
    vector<Connection>          Connections;    // input info
    vector<Contour_horizontal>  Contour_H;      // maintain contour to insert block efficiently
    vector<Contour_vertical>    Contour_V;      // maintain contour to insert block efficiently

};

#endif