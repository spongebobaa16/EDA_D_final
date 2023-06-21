#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Solver.h"
#include "Module.h"
#include "B_tree.h"

using namespace std;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    Solver s;
    s.readFile(argv[1]);
    // s.readFile_givenWL(argv[1]);
    //  s.printModules();

    B_Tree t;
    t.create_tree(s);
    t.initialTemp(s);
    
    // no fixed
    
    // do{
    //     t.fastSA5(s, 0.5);
    //     //t.printTree();
    // } while(s.OutofChip_y || s.OutofChip_x);
    
    // //t.fastSA5(s, 0.5);
    // cout<<"W: "<<s.HPWL<<endl;
    // cout<<"A: "<<s.A<<endl;
    // cout<<"floorplan x: "<<s.floorplan_x<<endl;
    // cout<<"floorplan y: "<<s.floorplan_y<<endl;
    // s.outputFloorPlan();
    
    // t.fastSA5(s, 0.5);
    
    
    do
    {
        t.SA(s, 0.5);
        cout << "W: " << s.HPWL << endl;
        cout << "A: " << s.A << endl;
        cout << "floorplan x: " << s.floorplan_x << endl;
        cout << "floorplan y: " << s.floorplan_y << endl;
        // if (!s.OutofChip_x && !s.OutofChip_y)
        //     cout << "valid" << endl;
        // do{
        //     t.SA(s);
        //     //t.printTree();
        // } while(s.OutofChip_y || s.OutofChip_x);
        t.prePlacedModule(s);
        s.outputFloorPlan();
        cout << "1: " << !s.checkOverlap() << " 2: " <<  s.OutofChip_y << " 3: " << s.OutofChip_x << endl; 
    } while (!s.checkOverlap() || s.OutofChip_y || s.OutofChip_x);
    
    s.outputFloorPlan();
    cout << '\a' << endl;
    
    
    // t.printTree();
    // s.floorplan(t, 0);
    // bool _first = 1;

    /*
    B_Tree t;
    t.create_tree(s);
    t.initialTemp(s);
    // do
    // {
        do
        {
            s.resetFixedStatus();
            t.fastSA5(s, 0.5);
            // t.printTree();
            // if (!_first)
            //     t.checkOverlap(s, 1);
            // _first = 0;
        } while (!t.prePlacedModule(s));
        // t.printTree();
        // bool dummy = 1;
        // s.floorplan(t, dummy, 1);
    // } while (!s.checkOverlap());


    // t.printTree();
    // t.prePlacedModule(s);
    t.printTree();
    t.printTreePreorder(s, t.root);
    cout << endl;
    // cout << "who do you wonder ? " << endl;
    // string _s;
    // cin >> _s;
    // Node *_Ret;
    // bool re = 0;
    // cout << t.Tree_vec[stoi(_s)]->getParent(s.Modules, _Ret, re) << ' ';
    // cout << _Ret->index << (re ? "true" : "false") << endl;

        // s.floorplan(t, 1);
        // s.outputFloorPlan();
        s.outputFloorPlanRect();
        cout << '\a' << endl;
        //     do
        //     {
        //         t.SA(s);
        //         t.printTree();

    //     } while (!t.prePlacedModule(s));
    //     s.floorplan(t);
    //     s.outputFloorPlan();

    */
}