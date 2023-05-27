#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include "Coord.h"
using namespace std;


class Module{
public: 
    Module(){}
    ~Module(){}

    int             index;      // self index for vector<Module> in Solver.h
    string          name;       // input info
    int             area;       // input info
    int             width;      // enclosing rectangle height
    int             height;     // enclosing rectangle height
    Coord           location;   // Bottom Left coordinate (x, y) of enclosing rectangle
    vector<Coord>   tour;       // Each corner coordinate (x, y). For output (clockwise order)       
    bool            fixed;      // fixed: true,  soft: false
};

#endif