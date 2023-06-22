#include <stdio.h>
#include <string.h>
#include "B_tree.h"
#include "Solver.h"
#include "util.h"
#include <unistd.h>
// #define N 10 //N times perturb before SA
//  #define P 0.99 //initial accept rate
#define P 0.999
#define K 10
#define c 100
#define k 5
#define frozen 0.001
#define ratio 0.85
#define lambdatf 0.005

void B_Tree::create_tree(const Solver &s)
{
    for (int i = 0; i < s.num_softmodules; i++)
    {
        Node *newNode = new Node(i);
        Tree_vec.push_back(newNode);
    }

    // Manually initial B_Tree below, for debugging

    root = Tree_vec[0];
    for (int i = 1; i < s.num_softmodules; i++)
    {
        if (i % 2 == 1)
        {
            insert(i, (i - 1) / 2, true, false);
        }
        else
        {
            insert(i, (i - 1) / 2, false, false);
        }
    }

    // remove(6, 0);
    printTree();
    // ─────0
    //     L├────1
    //      │   L├────3
    //      │    │   L├────7
    //      │    │   R└───8
    //      │   R└───4
    //      │       L├────9
    //     R└───2
    //         L├────5
    //         R└───6
}

void B_Tree::insert(int index, int parent, bool parent_left, bool child_left)
{
    int original_child = -1;
    if (parent_left)
    {
        if (Tree_vec[parent]->left != 0)
            original_child = Tree_vec[parent]->left->index;
        Tree_vec[parent]->left = Tree_vec[index];
    }
    else
    {
        if (Tree_vec[parent]->right != 0)
            original_child = Tree_vec[parent]->right->index;
        Tree_vec[parent]->right = Tree_vec[index];
    }

    Tree_vec[index]->parent = Tree_vec[parent];

    if (original_child != -1)
    {
        Tree_vec[original_child]->parent = Tree_vec[index];
        if (child_left)
            Tree_vec[index]->left = Tree_vec[original_child];
        else
            Tree_vec[index]->right = Tree_vec[original_child];
    }
}

// void B_Tree::rotate(int index)
// {
// }
void B_Tree::remove(int index, bool child_left) // if child_left == True -> swap with left child
{
    Node *_to_remove = Tree_vec[index];
    if (_to_remove == 0)
        return;
    if (_to_remove->left == 0 && _to_remove->right == 0) // case 1 : delete leaf node
    {
        if (_to_remove->parent != 0)
        {
            if (_to_remove->isLeftChild())
                _to_remove->parent->left = 0;
            else
                _to_remove->parent->right = 0;
        }
    }
    else if (_to_remove->left == 0 || _to_remove->right == 0) // case 2 : delete node with single child
    {
        Node *_to_remove_child = (_to_remove->right != 0) ? _to_remove->right : _to_remove->left;
        if (_to_remove == root)
            root = _to_remove_child;
        if (_to_remove->parent != 0)
        {
            if (_to_remove->isLeftChild())
                _to_remove->parent->left = _to_remove_child;
            else
                _to_remove->parent->right = _to_remove_child;
        }
        _to_remove_child->parent = _to_remove->parent;
    }
    else // case 3 : delete node with two children
    {
        bool isRootChosen = 0, isToRemoveRoot = (root == _to_remove);
        Node *newRoot = 0;
        while (_to_remove->left != 0 || _to_remove->right != 0)
        {
            Node *child_to_swap = child_left ? _to_remove->left : _to_remove->right;
            if (child_to_swap == 0)
                child_to_swap = !child_left ? _to_remove->left : _to_remove->right;
            swap(_to_remove->index, child_to_swap->index);
            if (!isRootChosen)
            {
                newRoot = Tree_vec[child_to_swap->index];
                isRootChosen = 1;
            }
        }
        if (isToRemoveRoot)
            root = newRoot;
        if (_to_remove->parent != 0)
            (_to_remove->isLeftChild() ? _to_remove->parent->left : _to_remove->parent->right) = 0;
    }
    Tree_vec[index] = 0;
    delete _to_remove;
}

/*
three cases:
    1.  no relationship
    2. one node is root
    3. there is parent/child relationship between node1 & node2 -> make node1 always the parent
*/

void B_Tree::swap(int index1, int index2) // deal with normal case first (check) , next root (check) , next nodes with parent/child relationship
{                                         // if node1 node2's number of children is not the same ? (check)
    Node *node1 = Tree_vec[index1], *node2 = Tree_vec[index2];
    if (node1->depth() > node2->depth()) // make node1 always the elder
    {
        Node *tmp1 = node1;
        node1 = node2;
        node2 = tmp1;
    }

    bool PCrelationship = node1->isChildOf(node2) || node2->isChildOf(node1); // if node1 & node2 have parent-child relationship
    bool node2Left = 0;
    if (node2->parent != 0)
        node2Left = node2->isLeftChild(); // see if node2 is left child or not
    Node *root_original = root;
    if (node1 == root || node2 == root)
        root = node1 == root ? node2 : node1;

    // swap parent

    // if (node1->isChildOf(node2) || node2->isChildOf(node1))
    // {
    //     PCrelationship = 1;
    //     if (node1->isChildOf(node2)) // make node1 always the parent
    //     {
    //         Node *tmp1 = node1;
    //         node1 = node2;
    //         node2 = tmp1;
    //     }
    // }
    Node *tmp = node1->parent;

    // parent_childi see nodei is its parent's left or right child
    Node *&parent_child1 = (node1 != root_original) ? ((node1->isLeftChild() ? node1->parent->left : node1->parent->right)) : dummy;
    Node *&parent_child2 = (node2 != root_original) ? ((node2->isLeftChild() ? node2->parent->left : node2->parent->right)) : dummy;

    if (!PCrelationship)
    {
        node1->parent = node2->parent;
        node2->parent = tmp;
        parent_child1 = node2; // original node1's parent's child should be update to node2
        parent_child2 = node1; // original node2's parent's child should be update to node1
    }
    else
    {
        node1->parent = node2;
        node2->parent = tmp;
        parent_child1 = node2; // original node1's child should be update to node2
        // (node2Left ? node2->left : node2->right) = node1; // original node2's child should be update to node1
    }

    // swap left child
    if (!PCrelationship || !node2Left)
    {
        tmp = node1->left;
        node1->left = node2->left;
        node2->left = tmp;
        if (node1->left != 0)
            node1->left->parent = node1; // node1's left child's parent should = node1
        if (node2->left != 0)
            node2->left->parent = node2; // node2's left child's parent should = node2
    }
    else
    {
        node1->left = node2->left;
        node2->left = node1;
        if (node1->left != 0)
            node1->left->parent = node1;
        // node1->parent is already node2
    }

    // swap right child
    if (!PCrelationship || node2Left)
    {
        tmp = node1->right;
        node1->right = node2->right;
        node2->right = tmp;
        if (node1->right != 0)
            node1->right->parent = node1; // node1's left child's parent should = node1
        if (node2->right != 0)
            node2->right->parent = node2; // node2's left child's parent should = node2
    }
    else
    {
        node1->right = node2->right;
        node2->right = node1;
        if (node1->right != 0)
            node1->right->parent = node1;
        // node1->parent is already node2
    }
}

void B_Tree::move(int index1, int index2, bool parent_left, bool child_left)
{

    remove(index1, 1);
    Tree_vec[index1] = new Node(index1);
    insert(index1, index2, parent_left, child_left);
}

int B_Tree::Width(int i, const Solver &s)
{
    return s.Modules[Tree_vec[i]->index]->width;
}

int B_Tree::Height(int i, const Solver &s)
{
    return s.Modules[Tree_vec[i]->index]->height;
}

void B_Tree::copyTree(vector<Node *> &a, int size)
{ // copy nodes in Tree_vec to a
    for (int i = 0; i < size; ++i)
    {
        // memmove(a[i], Tree_vec[i], sizeof(Node));
        a[i]->parent = Tree_vec[i]->parent;
        a[i]->left = Tree_vec[i]->left;
        a[i]->right = Tree_vec[i]->right;
        a[i]->index = Tree_vec[i]->index;
        a[i]->_isRotated = Tree_vec[i]->_isRotated;
        a[i]->WHtype = Tree_vec[i]->WHtype;
    }
    // // cout<<endl;
    return;
}

void B_Tree::returnTree(vector<Node *> a, int size)
{ // return nodes in a to Tree_vec
    for (int i = 0; i < size; ++i)
    {
        // memmove(Tree_vec[i], a[i], sizeof(Node));
        Tree_vec[i]->parent = a[i]->parent;
        Tree_vec[i]->left = a[i]->left;
        Tree_vec[i]->right = a[i]->right;
        Tree_vec[i]->index = a[i]->index;
        Tree_vec[i]->_isRotated = a[i]->_isRotated;
        Tree_vec[i]->WHtype = a[i]->WHtype;
    }
    // // cout<<endl;
    // // cout<<"finish ";
    // // cout.flush();
    return;
}

void B_Tree::fastSA5(Solver &s, float beta)
{
    float alpha = 0.5 / 4; // random, not important
    bool dummy = 1;

    Node *best_root;
    vector<Node *> best;
    for (int i = 0; i < s.num_softmodules; i++)
    {
        Node *newNode = new Node(i);
        best.push_back(newNode);
    }

    float best_HPWL = numeric_limits<float>::max();

    copyTree(best, s.num_softmodules);
    for (int i = 0; i < 5; ++i)
    {
        SA(s, beta);
        // cout<<i<<"th HPWL: "<<s.HPWL<<endl;
        // printTree();
        if (s.HPWL < best_HPWL)
        {
            copyTree(best, s.num_softmodules);
            best_root = root;
            best_HPWL = s.HPWL;
        }
    }
    returnTree(best, s.num_softmodules);
    // printTree();
    root = best_root;
    // cout << "root: m" << root->index + 1 << endl;
    s.Contour_H.clear();
    s.floorplan(*this, dummy);
    float cost = s.calculate_totalcost(alpha, beta);
    s.Contour_H.clear();
}

void B_Tree::SA(Solver &s, float beta)
{

    bool dummy = 1;
    // const float n = K * s.num_softmodules; // total number of uphill moves
    // const float frozen = T0 / 1.0e10;
    //  cout<<"initial: "<<T0<<endl;
    //  cout.flush();
    //  cout<<"frozen: "<<frozen<<endl;
    //  cout.flush();

    float alpha = 0.5 / 4; // random, not important
    // float beta=(1-alpha)/4;
    // float alpha_base=alpha;
    float beta_base = beta;

    float num_in_chip = 0.0;
    float new_cost = 0.0, delta_c;

    Node *best_root = root;
    vector<Node *> best, prev;
    int n_module = s.num_softmodules;
    for (int i = 0; i < n_module; i++)
    {
        Node *newNode = new Node(i);
        best.push_back(newNode);
    }
    for (int i = 0; i < n_module; i++)
    {
        Node *newNode = new Node(i);
        prev.push_back(newNode);
    }

    copyTree(best, n_module);
    copyTree(prev, n_module);

    s.floorplan(*this, dummy);
    float prev_cost = s.calculate_totalcost(alpha, beta);
    float best_cost = prev_cost;

    float T = T0;
    // float n_perturb=s.num_softmodules*s.num_softmodules*2;
    float n_perturb = float(2 * n_module + 20);
    float reset_th = float(2 * n_module);
    float stop_th = float(5 * n_module);
    float reject, reject_ratio;
    float iter = 1; // k-th iteration
    do
    {
        iter++;
        reject = 0;
        float total_delta_cost = 0.0;
        float num_feasible_floorplans = 0.0;
        // float min_delta_c=numeric_limits<float>::max();
        // float max_delta_c=0.0;

        for (int i = 0; i < n_perturb; ++i)
        {
            new_cost = perturb(s, alpha, beta);
            // cout<<"current cost: "<<new_cost<<endl;
            delta_c = new_cost - prev_cost;
            // cout<<"delta cost: "<<delta_c<<endl;
            total_delta_cost += abs(delta_c);

            if (delta_c <= 0)
            { // down-hill move
                // cout << "downhill" << new_cost << " " << prev_cost<< endl;

                if (!s.OutofChip_x && !s.OutofChip_y)
                {
                    // cout<<"feasible!"<<endl;
                    ++num_feasible_floorplans;
                    ++num_in_chip;
                    if (new_cost < best_cost)
                    {
                        copyTree(best, n_module);

                        // printTree();
                        best_root = root;
                        best_cost = new_cost;
                    }
                }
                // total_delta_cost += abs(delta_c);
                copyTree(prev, n_module);
                prev_cost = new_cost;
                // cout << "DOWN HILLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL" << endl;
            }
            else
            {                           // uphill move
                if (accept(delta_c, T)) // decide if we should accept the new tree
                {
                    // cout << "uphill" << new_cost << " " << prev_cost << endl;
                    // cout << "UPHILL ACCEPTEDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << endl;
                    // total_delta_cost += abs(delta_c);
                    copyTree(prev, n_module);
                    prev_cost = new_cost;
                }
                else
                {
                    reject++;
                    // // cout << "REJECTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << endl;
                }
            }
        }

        reject_ratio = reject / n_perturb;

        beta = beta_base + (1 - beta_base) * (num_feasible_floorplans / n_perturb);
        // alpha = alpha_base + (alpha - alpha_base)*(num_feasible_floorplans / n_perturb);
        // beta = beta_base + (beta - beta_base)*(num_feasible_floorplans / n_perturb);
        // cout<<"feasible: "<<num_feasible_floorplans<<endl;
        // cout<<"beta: "<<beta<<endl;

        float average_delta_cost = abs(total_delta_cost / n_perturb);
        // cout<<"avg delta cost: "<<average_delta_cost<<endl;

        // update temperature
        if (iter >= 2 && iter <= k)
        {
            T = T0 * average_delta_cost / iter / c;
            // cout<<"stage 2"<<endl;
        }
        else
        {
            T = T0 * average_delta_cost / iter;
            // cout<<"stage 3"<<endl;
        }
        // cout << "T0: " << T0 << endl;
        // cout<<iter<<" "<<T<<" "<<frozen<<endl;
        // if (T < frozen)
        // cout << iter << " FROzeee" << endl;
        // cout << !s.checkOverlap() << " " << s.OutofChip_y << " " << s.OutofChip_x << endl;
        // cout << (!s.checkOverlap() || s.OutofChip_y || s.OutofChip_x) << endl;
        // cout<<endl;
        // cout.flush();

        if (num_in_chip == 0)
        {
            if (iter > reset_th)
            {
                T = T0;
                iter = 1;
                // cout<<"RESET TEMPPPPPPPPPPPPPPPPPPPPPPPPPPP"<<endl;
                reset_th++;
                stop_th++;
                n_perturb++;
            }
        }
        else if (iter > stop_th)
        {
            cout << "IM DONEEEEEEEEEEEEEEEEEEEE" << endl;
            break;
        }

    } while ((reject_ratio <= 0.95 && T >= frozen) || num_in_chip == 0);
    // cout << "reject: " << reject << endl;
    // cout << "n_perturb: " << n_perturb << endl;

    returnTree(best, n_module);
    root = best_root;
    // cout << "root: m" << root->index + 1 << endl;
    s.Contour_H.clear();
    s.floorplan(*this, dummy);
    float cost = s.calculate_totalcost(alpha, beta);

    // cout << "best cost: " << best_cost << endl;
    // cout << "calculate: " << cost << endl;
    // cout << "HPWL: " << s.HPWL << endl;
    s.Contour_H.clear();
    // for(auto i:Tree_vec)
    //     // cout<<i->index<<endl;
    // exit(0);
    // printTree();
}

bool B_Tree::accept(int delta_c, float T)
{
    float prob = exp(-delta_c / T);
    // // cout << "prob: " << prob << endl;
    double r = ((double)rand() / (RAND_MAX));
    // // cout << "r: " << r << endl;
    if (r < prob)
    {
        return true; // accept
    }
    else
    {
        return false;
    }
}

float B_Tree::perturb(Solver &s, float alpha, float beta)
{
    int op = rand() % 4 + 1;
    // if (op == 1)
    // {
    //     int m1 = rand() % (s.num_softmodules);
    //     //cout << "op1: rotate " << m1 << endl;
    //     rotate(m1);
    //     // rotate(rand()%(s.num_softmodules));
    // }
    float r = ((float)rand() / (RAND_MAX));
    if (op == 1)
    {
        int m1 = rand() % (s.num_softmodules);
        if (s.OutofChip_y && ((Tree_vec[m1]->_isRotated && Width(m1, s) > Height(m1, s)) || (!Tree_vec[m1]->_isRotated && Width(m1, s) < Height(m1, s))))
        {
            if (r > 0.8)
            {
                rotate(m1);
                // cout << "op1: rotate " << m1 << endl;
            }
        }
        else if (s.OutofChip_x && ((Tree_vec[m1]->_isRotated && Width(m1, s) < Height(m1, s)) || (!Tree_vec[m1]->_isRotated && Width(m1, s) > Height(m1, s))))
        {
            if (r > 0.8)
            {
                rotate(m1);
                // cout << "op1: rotate " << m1 << endl;
            }
        }
        // cout << "op1: rotate " << m1 << endl;
        else
            rotate(m1);
    }
    // else if (op == 2)
    // {
    //     bool parent_left = (rand() % 2 == 1) ? true : false;
    //     bool child_left = (rand() % 2 == 1) ? true : false;
    //     int m1 = rand() % (s.num_softmodules);
    //     int m2;
    //     do
    //     {
    //         m2 = rand() % (s.num_softmodules);
    //     } while (m1 == m2);

    //     //cout << "op2: move " << m1 << " to " << m2 << endl;
    //     move(m1, m2, parent_left, child_left);
    // }
    else if (op == 2)
    {
        bool parent_left;
        if (s.OutofChip_y)
            parent_left = (r > 0.2) ? true : false;
        else if (s.OutofChip_x)
            parent_left = (r > 0.8) ? true : false;
        else
            parent_left = (r >= 0.5) ? true : false;

        bool child_left = (rand() % 2 == 1) ? true : false;
        int m1 = rand() % (s.num_softmodules);
        int m2;
        do
        {
            m2 = rand() % (s.num_softmodules);
        } while (m1 == m2);

        // cout << "op2: move " << m1 << " to " << m2 << endl;
        move(m1, m2, parent_left, child_left);
    }
    else if (op == 3)
    {
        int m1 = rand() % (s.num_softmodules);
        int m2;
        do
        {
            m2 = rand() % (s.num_softmodules);
        } while (m1 == m2);
        // // cout << "op3: swap " << m1 << " and " << m2 << endl;
        swap(m1, m2);
    }
    else if (op == 4)
    {
        int m1 = rand() % (s.num_softmodules);
        s.Modules[Tree_vec[m1]->index]->changeWH(rand());
    }

    bool dummy = 1;
    s.floorplan(*this, dummy);
    float cost = s.calculate_totalcost(alpha, beta);
    s.Contour_H.clear();
    // if(!s.OutofChip_x&&!s.OutofChip_y){
    // cout<<"yeahhhhhhhhhhhhhhhhhhh"<<endl;
    // }
    // else
    // cout<<"nooooooooooooooooooooo"<<endl;

    // printTree();
    return cost;
}

void B_Tree::init_perturb(Solver &s)
{
    int op = rand() % 4 + 1;
    if (op == 1)
    {
        int m1 = rand() % (s.num_softmodules);
        // cout << "op1: rotate " << m1 << endl;
        rotate(m1);
        // rotate(rand()%(s.num_softmodules));
    }

    else if (op == 2)
    {
        bool parent_left = (rand() % 2 == 1) ? true : false;
        bool child_left = (rand() % 2 == 1) ? true : false;
        int m1 = rand() % (s.num_softmodules);
        int m2;
        do
        {
            m2 = rand() % (s.num_softmodules);
        } while (m1 == m2);

        // cout << "op2: move " << m1 << " to " << m2 << endl;
        move(m1, m2, parent_left, child_left);
    }

    else if (op == 3)
    {
        int m1 = rand() % (s.num_softmodules);
        int m2;
        do
        {
            m2 = rand() % (s.num_softmodules);
        } while (m1 == m2);
        // // cout << "op3: swap " << m1 << " and " << m2 << endl;
        swap(m1, m2);
    }
    else if (op == 4)
    {
        int m1 = rand() % (s.num_softmodules);
        s.Modules[Tree_vec[m1]->index]->changeWH(rand());
    }

    bool dummy = 1;
    s.floorplan(*this, dummy);
    s.calculate_area_wirelength();
    s.Contour_H.clear();

    // printTree();
}

void B_Tree::initialTemp(Solver &s)
{
    const int N = s.num_softmodules * s.num_softmodules; // N times perturb before SA
    float total_A = 0;                                   // to calculate A_norm
    float total_HPWL = 0;                                // to calculate HPWL_norm
    float total_area_penalty = 0;
    for (int i = 0; i < N; ++i)
    {
        init_perturb(s);
        if (s.A < s.A_min)
            s.A_min = s.A;
        if (s.A > s.A_max)
            s.A_max = s.A;
        if (s.HPWL < s.HPWL_min)
            s.HPWL_min = s.HPWL;
        if (s.HPWL > s.HPWL_max)
            s.HPWL_max = s.HPWL;

        total_A += s.A;
        total_HPWL += s.HPWL;
        total_area_penalty += s.area_penalty;
        // printTree();
    }
    s.A_norm = total_A / float(N);
    s.HPWL_norm = total_HPWL / float(N);
    s.area_penalty_norm = total_area_penalty / float(N);

    float uphill_cost = 0;
    float alpha = 0.5 / 4;
    float beta = 0.5;
    float prev_cost = perturb(s, alpha, beta);
    int uphill_times = 0; // counting number of uphill moves

    do
    {
        for (int i = 1; i < N; ++i)
        {
            float cost = perturb(s, alpha, beta);
            // if (cost > prev_cost)
            // {
            uphill_cost += abs(cost - prev_cost);
            uphill_times++;
            // cout<<"UPHILL"<<endl;
            // }
            prev_cost = cost;
            // printTree();
        }
    } while (uphill_times == 0);

    float c_avg = uphill_cost / float(uphill_times); // calculates average difference of uphill moves
    // cout<<"c_avg: "<<c_avg<<endl;
    // cout<<"uphill_times: "<<uphill_times<<endl;
    T0 = -c_avg / log(P); // calculates initial temperature based on this formula, P being the initial probability of uphill moves
    cout << "initial temperature: " << T0 << endl;
}

bool B_Tree::prePlacedModule(Solver &s) // fixed module is root???  // if the place is empty where fixed module wants to go??
{                                       // D = {} ??    // heuristic -> the floorplan usually not good when D.size() == 1
    // printTree();                     // cannot place fixed module with big height so early
    s.outputFloorPlan(0);
    int cnt = 2;
    for (auto i : s.fixedModules)
    {
        i->location = i->fix_location;
    }

    bool _enable = 0;
    printTreePreorder(s, root);
    cout << endl;
    printTree();
    s.floorplan((*this), _enable, 1); // cannot skip previous step!
    return 1;
}
void B_Tree::exchangableNode(Solver &s, Node *_node, Node *_fixed, vector<Node *> &D, size_t _specificDirection) // specificDirection : 0 -> both, 1 -> left only, 2 -> right only
{
    if (_node == 0 || (!s.Modules[_node->index]->fixed && !s.Modules[_node->index]->isDominated(s.Modules[_fixed->index]))) // ignore case with two fixed module
        return;
    if (!s.Modules[_node->index]->fixed)
        D.push_back(_node);
    if (_specificDirection == 1 || _specificDirection == 0)
        exchangableNode(s, _node->left, _fixed, D);
    if (_specificDirection == 2 || _specificDirection == 0)
        exchangableNode(s, _node->right, _fixed, D);
}

// bool B_Tree::checkOverlap(Solver &s, bool _toPlaceLeft)
// {
//     vector<Module *> fixedModules;
//     for (size_t i = s.num_softmodules - 1; i >= 0 && s.Modules[i]->fixed; --i)
//         fixedModules.push_back(s.Modules[i]);
//     for (auto fixModule : fixedModules)
//     {
//         for (size_t i = 0, n = s.num_softmodules; i < n; ++i)
//         {
//             if (s.Modules[i]->fixed)
//                 continue;
//             if (s.Modules[i]->isOverlap(fixModule))
//             {
//                 // cout << i << " & " << fixModule->index << " overlap!!!" << endl;
//                 return 0;
//             }
//         }
//     }
//     return 1;
// }

void B_Tree::printTreePreorder(Solver &s, Node *node)
{ // for debug
    if (node == NULL)
    {
        return;
    }

    // cout << node->index << " ";
    cout << s.Modules[node->index]->name << ' ';

    // // cout << node->index << " ";
    // if (node->parent != NULL)
    //     cout << " parent: " << node->parent->index << endl;
    // else
    //     cout << "NO parent" << endl;
    printTreePreorder(s, node->left);
    printTreePreorder(s, node->right);
}

void B_Tree::printTree(const string &prefix, Node *parent, bool isLeft, bool isRoot)
{
    if (parent == nullptr)
        return;

    if (isRoot)
    {
        cout << "─────";
    }
    else
    {
        cout << prefix << (isLeft ? "L├────" : "R└───");
    }
    cout << parent->index << endl;

    printTree(prefix + (isLeft ? " │   " : "    "),
              parent->left, true, false);
    printTree(prefix + (isLeft ? " │   " : "    "),
              parent->right, false, false);
}