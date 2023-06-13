#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Coord.h"
using namespace std;

class Module
{
public:
    Module() {}
    ~Module() {}

    int index;          // self index for vector<Module> in Solver.h
    string name;        // input info
    int area;           // input info
    int width;          // enclosing rectangle height
    int height;         // enclosing rectangle height
    Coord location;     // Bottom Left coordinate (x, y) of enclosing rectangle
    Coord fix_location; // Bottom Left coordinate (x, y) of enclosing rectangle (fixed module's given location)
    vector<Coord> tour; // Each corner coordinate (x, y). For output (clockwise order)
    bool fixed;         // fixed: true,  soft: false
    vector<Width_Height> validWH;
    void calculate_w_l(int area);
    void rotate()
    {
        int tmp = width;
        width = height;
        height = tmp;
    }

    void changeWH(int num)
    {
        if (validWH.size() > 0)
        {
            width = validWH[num % validWH.size()].width;
            height = validWH[num % validWH.size()].height;
        }
    }

    bool isDominated(Module *_fixed)
    {
        if (fixed)
            return (_fixed->fix_location.x >= fix_location.x && _fixed->fix_location.y >= fix_location.y);
        return (_fixed->fix_location.x >= location.x && _fixed->fix_location.y >= location.y);
    }
    size_t manhattanDistance_LC(Module *_fixed) // manhattan distance between this and _n 's left down corner
    {
        return abs(location.x - _fixed->fix_location.x) + abs(location.y - _fixed->fix_location.y);
    }
};

#endif