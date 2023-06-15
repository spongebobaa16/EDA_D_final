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

using namespace std;

class Solver
{
public:
    Solver()
    {
        OutofChip_y = false;
        OutofChip_x = false;
    }
    ~Solver() {}

    void readFile(const char *);
    // Width_Height    calculate_w_l(int area);            // calculate feasible width for area
    void floorplan(B_Tree t, bool &_enable, bool isFixedMode = 0, Node *_target = 0); // calculate floorplan with given B_Tree and Modules, change each Module's location property
    // _target node is for searching specific node(so i can ignore the node chronologically earlier found)
    void placeBlock(Node *node, int type, bool isFixedMode, bool &_enable, Node *_target = 0); // floorplan(B_Tree t) will recursively call placeBlock with preorder, type: 0->root, 1->left, 2->right
    float calculate_totalcost();                                                               // calculate floorplan's total cost
    int findYandUpdateContour_H(int index, int from_x, int to_x);                              // return Y coordinate for block and maintain vector Contour_H, from_x~to_x is the x range this block is going to placed
    int findYandUpdateContour_H_fixed(int index, int from_x, int to_x);                        // return Y coordinate for block and maintain vector Contour_H, from_x~to_x is the x range this block is going to placed
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
    void setChip()
    {
        chip->location.x = 0;
        chip->location.y = 0;
        chip->height = chip_height;
        chip->width = chip_width;
    }
    bool OutofChip_y;
    bool OutofChip_x;
    int num_softmodules;
    int num_fixedmodules;
    int chip_width;  // input info
    int chip_height; // input info
    float HPWL;
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
    void outputFloorPlanRect();
};

#endif