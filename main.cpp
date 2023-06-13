#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "Solver.h"
#include "Module.h"
#include "B_tree.h"

<<<<<<< HEAD

=======
#define N 10
#define P 0.99
// #define
>>>>>>> b1661a5f334198cf72779b800ed5da8562708ff5
using namespace std;

float perturb(B_Tree &t, Solver &s)
{
    int op = rand() % 3 + 1;
    if (op == 1)
    {
        int m1 = rand() % (s.Modules.size());
        cout << "op1: rotate " << m1 << endl;
        // t.printTree();
        t.rotate(m1);
        // t.rotate(rand()%(s.Modules.size()));
    }
    else if (op == 2)
    {
        bool parent_left = (rand() % 2 == 1) ? true : false;
        bool child_left = (rand() % 2 == 1) ? true : false;
        int m1 = rand() % (s.Modules.size());
        int m2;
        do
        {
            m2 = rand() % (s.Modules.size());
        } while (m1 == m2);

        cout << "op2: move " << m1 << " to " << m2 << endl;
        // t.printTree();
        t.move(m1, m2, parent_left, child_left);
    }
    else if (op == 3)
    {
        int m1 = rand() % (s.Modules.size());
        int m2;
        do
        {
            m2 = rand() % (s.Modules.size());
        } while (m1 == m2);
        cout << "op3: swap " << m1 << " and " << m2 << endl;
        // t.printTree();
        t.swap(m1, m2);
    }
    s.floorplan(t);
    double cost = s.calculate_totalcost();
    s.Contour_H.clear();
    return cost;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    Solver s;
    s.readFile(argv[1]);
    // s.readFile_givenWL(argv[1]);
    //  s.printModules();
    B_Tree t;
    t.create_tree(s);
    // t.printTreePreorder(t.root);
    // s.floorplan(t);
    // s.printLocations();

    // test
    //  for(int i=700; i< 705; i++){
    //      Module* a = new Module();
    //      a->calculate_w_l(i);
    //  }

    // cout<<"cost: "<<s.calculate_totalcost()<<endl;
    // s.outputFloorPlan();

    // float T0=t.initialTemp(s);
    // cout<<"yayyyyy: "<<T0<<endl;
    // t.printTree();
    do
    {
        t.SA(s);
        t.printTree();

    } while (!t.prePlacedModule(s));
    s.floorplan(t);
    s.outputFloorPlan();
}