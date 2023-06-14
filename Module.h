#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <iostream>
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
    bool coordWithin(Coord _point, Coord *_assume)
    {
        if (_assume == 0)
            return (_point.x > location.x && _point.x < (location.x + width)) && (_point.y > location.y && _point.y < (location.y + height));
        else
            return (_point.x > _assume->x && _point.x < (_assume->x + width)) && (_point.y > _assume->y && _point.y < (_assume->y + height));
    }
    bool isOverlap(Module *_fixed, Coord *_assume = 0) // _assume is to check if i placed this module at _assume, will overlap happens?
    {                                                  // right_bottom corner, left top corner, right top corner
        Coord rb(_fixed->fix_location.x + _fixed->width, _fixed->fix_location.y),
            lt(_fixed->fix_location.x, _fixed->fix_location.y + _fixed->height), rt(_fixed->fix_location.x + _fixed->width, _fixed->fix_location.y + _fixed->height);
        if (coordWithin(_fixed->fix_location, _assume))
        {
            cout << "lb overlap!!!" << endl;
            return 1;
        }
        else if (coordWithin(rb, _assume))
        {
            cout << "rb overlap!!!" << endl;
            return 1;
        }
        else if (coordWithin(lt, _assume))
        {
            cout << "lt overlap!!!" << endl;
            return 1;
        }
        else if (coordWithin(rt, _assume))
        {
            cout << "rt overlap!!!" << endl;
            return 1;
        }
        return 0;
        // return coordWithin(_fixed->fix_location, _assume) || coordWithin(rb, _assume) || coordWithin(lt, _assume) || coordWithin(rt, _assume);
    }
};

#endif