#include <iostream>
#include <fstream>
#include "Solver.h"
#include "Module.h"
#include "B_tree.h"

using namespace std;


int main(int argc, char *argv[]){
    Solver s;
    s.readFile_givenWL(argv[1]);
    // s.printModules();

    B_Tree t;
    t.create_tree(s);
    // t.printTreePreorder(t.root);
    s.floorplan(t);
    //s.printLocations();
    cout<<"Area: "<<s.calculate_totalcost()<<endl;
    s.outputFloorPlan();
}