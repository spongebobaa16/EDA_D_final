#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
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

    t.SA(s);
    t.printTree();
    s.floorplan(t);
    s.outputFloorPlan();

//     do
//     {
//         t.SA(s);
//         t.printTree();

//     } while (!t.prePlacedModule(s));
//     s.floorplan(t);
//     s.outputFloorPlan();
}