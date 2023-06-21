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
#include <iomanip>
#include <limits>

using namespace std;

class Solver
{
public:
    Solver()
    {
        OutofChip_y = false;
        OutofChip_x = false;
        A_min = numeric_limits<float>::max();
        A_max = 0.0;
        HPWL_min = numeric_limits<float>::max();
        HPWL_max = 0.0;
    }
    ~Solver() {}

    void readFile(const char *);
    // Width_Height    calculate_w_l(int area);            // calculate feasible width for area
    void floorplan(B_Tree &t, bool &_enable, bool isFixedMode = 0); // calculate floorplan with given B_Tree and Modules, change each Module's location property
    // _target node is for searching specific node(so i can ignore the node chronologically earlier found)
    // change parent = 1 if the node's parent is fixed and it wants to change parent
    void placeBlock(Node *node, int type, bool isFixedMode); // floorplan(B_Tree t) will recursively call placeBlock with preorder, type: 0->root, 1->left, 2->right
    float calculate_totalcost(float alpha, float beta);      // calculate floorplan's total cost
    void calculate_area_wirelength();
    int findYandUpdateContour_H(int index, int from_x, int to_x);       // return Y coordinate for block and maintain vector Contour_H, from_x~to_x is the x range this block is going to placed
    int findYandUpdateContour_H_fixed(int index, int from_x, int to_x); // return Y coordinate for block and maintain vector Contour_H, from_x~to_x is the x range this block is going to placed
    void IsOutofChip();
    int findY(int index, int from_x, int to_x);
    int UpdateContour_H(int index, int from_x, int to_x, Module *fixedBlock = 0); // fixedBlock != 0 -> update contour to its height
    void resetFixedStatus()
    {
        for (auto i : fixedModules)
            i->fixed_status = 1;
    };
    void restoreFixedNode(B_Tree &t)
    {
        bool rand1 = rand() % 2, rand2 = rand() % 2;
        for (auto i : fixedModules)
            t.insert(i->index, t.root->index, rand1, rand2);
    }
    void placeAtRight(int &from_x, int &to_x, int &Yloc, int node_index, Module *&beyondBlock, Module *fixedModule, Coord *&_assume)
    {
        from_x = fixedModule->fix_location.x + fixedModule->width;
        to_x = from_x + Modules[node_index]->width;
        Yloc = findY(node_index, from_x, to_x);
        delete _assume;
        _assume = new Coord(from_x, Yloc);
        beyondBlock = 0;
    }
    void placeAtTop(int &from_x, int &to_x, int &Yloc, int node_index, Module *beyondBlock, Module *&fixedModule, Coord *&_assume)
    {
        // from_x = fixedModule->fix_location.x;
        from_x = from_x;
        to_x = from_x + Modules[node_index]->width;
        Yloc = fixedModule->fix_location.y + fixedModule->height;
        beyondBlock = fixedModule;
        delete _assume;
        _assume = new Coord(from_x, Yloc);
    }
    void randomPlacement(int &from_x, int &to_x, int &Yloc, int node_index, Module *fixedModule, Module *&beyondBlock, Coord *&_assume, Coord *&_current)
    {
        if (fixedModule->isSlim())
            placeAtRight(from_x, to_x, Yloc, node_index, beyondBlock, fixedModule, _assume);
        else
            placeAtTop(from_x, to_x, Yloc, node_index, beyondBlock, fixedModule, _assume);
        if (isOverlap_specificCoord(Modules[node_index], _assume) != -1) // if overlap
        {
            from_x = _current->x;
            Yloc = _current->y;
            delete _assume;
            _assume = new Coord(from_x, Yloc);
            if (!fixedModule->isSlim())
                placeAtRight(from_x, to_x, Yloc, node_index, beyondBlock, fixedModule, _assume);
            else
                placeAtTop(from_x, to_x, Yloc, node_index, beyondBlock, fixedModule, _assume);
            if (isOverlap_specificCoord(Modules[node_index], _assume) != -1) // if no overlap break
            {
                bool r = rand() % 2;
                from_x = _current->x;
                Yloc = _current->y;
                delete _assume;
                _assume = new Coord(from_x, Yloc);
                beyondBlock = 0;
                if (r)
                    placeAtRight(from_x, to_x, Yloc, node_index, beyondBlock, fixedModule, _assume);
                else
                    placeAtTop(from_x, to_x, Yloc, node_index, beyondBlock, fixedModule, _assume);
                int collision = isOverlap_specificCoord(Modules[node_index], _assume);
                if (collision != -1)
                {
                    _current->x = _assume->x;
                    _current->y = _assume->y;
                    randomPlacement(from_x, to_x, Yloc, node_index, fixedModules[collision], beyondBlock, _assume, _current);
                }
            }

        } // break;
    }
    int isOverlap_specificCoord(Module *_module, Coord *_assume = 0) // if exist overlap -> return 1; else return 0;
    {
        for (int i = 0, n = num_fixedmodules; i < n; ++i)
        {
            if (_module->isOverlap(fixedModules[i], _assume))
                return i;
        }
        return -1;
    }
    void setChip()
    {
        chip->location.x = 0;
        chip->location.y = 0;
        chip->height = chip_height;
        chip->width = chip_width;
    }
    bool OutofChip_y;
    bool OutofChip_x;
    int floorplan_y;
    int floorplan_x;
    int num_softmodules;
    int num_fixedmodules;
    int chip_width;  // input info
    int chip_height; // input info
    float A;         // area of the current floorplan
    float HPWL;      // wirelength of the current floorplan
    float area_penalty;
    float A_norm;            // for SA cost function
    float HPWL_norm;         // for SA cost function
    float area_penalty_norm; // for SA cost function
    float A_min;
    float A_max;
    float HPWL_min;
    float HPWL_max;

    unordered_map<string, int> U_Name_Index; // for all modules
    vector<Module *> Modules, fixedModules;  // input info, for both soft and fixed modules // fixedModules is for fix module only
    vector<Node *> Nodes;                    // store every Node*'s info, for search convenience
    vector<Connection> Connections;          // input info
    vector<Contour_horizontal> Contour_H;    // maintain contour to insert block efficiently
    Module *chip;                            // to check if compact enough
    // vector<Contour_vertical>    Contour_V;      // maintain contour to insert block efficiently

    // DEBUG FUNCTIONS // for w and l given.
    void readFile_givenWL(const char *); // ONLY FOR DEBUGGING, GIVEN SOFT BLOCKS' WIDTH AND HEIGHT
    void printModules();
    void printLocations();
    void outputFloorPlan(int isPrePlaced = 1);
    // to the rightmost end of B* tree, either right/left, depends on _toPlaceLeft = 0 / 1
    //  current solution : if overlap -> SA again  (since overlap doesn't happen that frequently)
    bool checkOverlap();
    void outputFloorPlanRect(bool kk = 1);
};

#endif