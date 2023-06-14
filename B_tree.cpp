#include <stdio.h>
#include <string.h>
#include "B_tree.h"
#include "Solver.h"
#include "util.h"
#include <unistd.h>
#define N 10
#define P 0.99
#define K 10
#define epsilon 0.001
#define ratio 0.85
#define lambdatf 0.005

void B_Tree::create_tree(const Solver &s)
{
    for (int i = 0; i < s.Modules.size(); i++)
    {
        Node *newNode = new Node(i);
        Tree_vec.push_back(newNode);
    }

    // Manually initial B_Tree below, for debugging

    root = Tree_vec[0];
    for (int i = 1; i < s.Modules.size(); i++)
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
    // printTree();
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
    // cout<<endl;
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
    // cout<<endl;
    // cout<<"finish ";
    // cout.flush();
    return;
}

void B_Tree::SA(Solver &s)
{
    float new_cost = 0, delta_c;

    const int n = K * s.Modules.size(); // total number of uphill moves
    float T0 = initialTemp(s);

    Node *best_root = root;
    vector<Node *> best, prev;
    for (int i = 0; i < s.Modules.size(); i++)
    {
        Node *newNode = new Node(i);
        best.push_back(newNode);
    }
    for (int i = 0; i < s.Modules.size(); i++)
    {
        Node *newNode = new Node(i);
        prev.push_back(newNode);
    }

    copyTree(best, s.Modules.size());
    copyTree(prev, s.Modules.size());

    s.floorplan(*this);
    float prev_cost = s.calculate_totalcost();
    float best_cost = prev_cost;

    float T = T0;
    float nmoves, uphill, reject, reject_ratio;
    int k = 0;
    // cout<<"what???"<<endl;
    do
    {
        nmoves = 0;
        uphill = 0;
        reject = 0;
        // cout<<"fuck"<<endl;
        do
        {
            new_cost = perturb(s);
            delta_c = new_cost - prev_cost;

            if (delta_c <= 0)
            { // down-hill movep
                // cout << "downhill" << new_cost << " " << prev_cost<< endl;
                if (new_cost < best_cost)
                {
                    copyTree(best, s.Modules.size());

                    // printTree();
                    // returnTree(best, s.Modules.size());
                    best_root = root;
                    // printTree();
                    // return;

                    best_cost = new_cost;
                    // cout<<"best cost: "<<best_cost<<endl;
                }
                copyTree(prev, s.Modules.size());
                prev_cost = new_cost;
                // cout << "DOWN HILLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL" << endl;
            }
            else
            {                           // uphill move
                if (accept(delta_c, T)) // decide if we should accept the new tree
                {
                    // cout << "uphill" << new_cost << " " << prev_cost << endl;
                    // cout << "UPHILL ACCEPTEDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << endl;
                    copyTree(prev, s.Modules.size());
                    prev_cost = new_cost;
                    uphill++;
                }
                else
                {
                    reject++;
                    // cout << "REJECTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << endl;
                }
            }

            nmoves++;
        } while (uphill < n && nmoves <= 2 * n);

        reject_ratio = reject / nmoves;

        if (k == 20)
            T = 0.6 * T0;
        else
            T = ratio * T;
        // T=T<lambdatf*T0 ? 0.1*T : ratio*T;

        k++;

    } while (reject_ratio <= 0.95 && T >= epsilon);
    cout << "reject: " << reject << endl;
    cout << "nmoves: " << nmoves << endl;

    returnTree(best, s.Modules.size());
    root = best_root;
    cout << "root: m" << root->index + 1 << endl;
    s.Contour_H.clear();
    s.floorplan(*this);
    float cost = s.calculate_totalcost();

    cout << "best cost: " << best_cost << endl;
    cout << "calculate: " << cost << endl;
    cout << "HPWL: " << s.HPWL << endl;
    s.Contour_H.clear();
    // for(auto i:Tree_vec)
    //     cout<<i->index<<endl;
    // exit(0);
    // printTree();
}

bool B_Tree::accept(int delta_c, float T)
{
    float prob = exp(-delta_c / T);
    // cout << "prob: " << prob << endl;
    double r = ((double)rand() / (RAND_MAX));
    // cout << "r: " << r << endl;
    if (r < prob)
    {
        return true; // accept
    }
    else
    {
        return false;
    }
}

float B_Tree::perturb(Solver &s)
{
    int op = rand() % 3 + 1;
    if (op == 1)
    {
        int m1 = rand() % (s.Modules.size());
        // cout << "op1: rotate " << m1 << endl;
        rotate(m1);
        // rotate(rand()%(s.Modules.size()));
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

        // cout << "op2: move " << m1 << " to " << m2 << endl;
        move(m1, m2, parent_left, child_left);
    }
    else if (op == 3)
    {
        int m1 = rand() % (s.Modules.size());
        int m2;
        do
        {
            m2 = rand() % (s.Modules.size());
        } while (m1 == m2);
        // cout << "op3: swap " << m1 << " and " << m2 << endl;
        swap(m1, m2);
    }
    s.floorplan(*this);
    float cost = s.calculate_totalcost();
    s.Contour_H.clear();

    // printTree();
    return cost;
}

float B_Tree::initialTemp(Solver &s)
{
    float uphill_cost = 0;
    float prev_cost = perturb(s);
    int uphill_times = 0; // counting number of uphill moves

    for (int i = 1; i < N; ++i)
    {
        float cost = perturb(s);
        if (cost > prev_cost)
        {
            uphill_cost += (cost - prev_cost);
            uphill_times++;
            // // cout<<"UPHILL"<<endl;
        }
        prev_cost = cost;
        // printTree();
    }

    float c_avg = uphill_cost / uphill_times; // calculates average difference of uphill moves
    float T0 = -c_avg / log(P);               // calculates initial temperature based on this formula, P being the initial probability of uphill moves
    cout << "initial temperature: " << T0 << endl;
    return T0;
}
// if fixed module is placed later, might overlap with other
bool B_Tree::prePlacedModule(Solver &s) // fixed module is root???  // if the place is empty where fixed module wants to go??
{                                       // D = {} ??    // heuristic -> the floorplan usually not good when D.size() == 1
    // printTree();                     // cannot place fixed module with big height so early
    vector<Module *> fixedModules;
    s.outputFloorPlan(0);
    for (size_t i = s.Modules.size() - 1; i >= 0 && s.Modules[i]->fixed; --i)
        fixedModules.push_back(s.Modules[i]);
    for (auto i : fixedModules)
    {
        Node *fixedNode = Tree_vec[i->index], *_it = fixedNode->parent, *firstDominatedNode = 0, *_prev = fixedNode; // _prev is to record which subtree does fixed node climb up from
        while (_it != 0)
        {
            if (s.Modules[_it->index]->isDominated(s.Modules[fixedNode->index]))
            {
                break;
            }
            if (_it == root)
                break;
            _prev = _it;
            _it = _it->parent;
        }
        firstDominatedNode = Tree_vec[i->index] == root ? root : _it;
        cout << fixedNode->index << ' ' << s.Modules[fixedNode->index]->fix_location.x << ' ' << s.Modules[fixedNode->index]->fix_location.y << endl;
        cout << firstDominatedNode->index << ' ' << s.Modules[firstDominatedNode->index]->location.x << ' ' << s.Modules[firstDominatedNode->index]->location.y << endl;
        cout << _prev->isLeftChild() << endl;
        vector<Node *> D;
        exchangableNode(s, firstDominatedNode, fixedNode, D, (Tree_vec[i->index] == root ? 0 : (_prev->isLeftChild() ? 2 : 1))); // from which subtree -> search the other
        if (D.empty())
            return 0;
        // // cout << "D = \n";
        // for (auto j : D)
        //     // cout << j->index << ' ';
        // // cout << endl;
        int closestDistance = 2147483647, closestIndex = -1;
        for (auto j : D)
        {
            // cout << s.Modules[j->index]->manhattanDistance_LC(s.Modules[fixedNode->index]) << ' ';
            if (s.Modules[j->index]->manhattanDistance_LC(s.Modules[fixedNode->index]) < closestDistance)
            {
                closestDistance = s.Modules[j->index]->manhattanDistance_LC(s.Modules[fixedNode->index]);
                closestIndex = j->index;
            }
        }
        cout << endl;
        cout << closestIndex << endl
             << endl;
        swap(i->index, closestIndex);
    }
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

bool B_Tree::checkOverlap(Solver &s, bool _toPlaceLeft)
{
    vector<Module *> fixedModules;
    for (size_t i = s.Modules.size() - 1; i >= 0 && s.Modules[i]->fixed; --i)
        fixedModules.push_back(s.Modules[i]);
    for (auto fixModule : fixedModules)
    {
        for (size_t i = 0, n = s.Modules.size(); i < n; ++i)
        {
            if (s.Modules[i]->fixed)
                continue;
            if (s.Modules[i]->isOverlap(fixModule))
            {
                cout << i << " & " << fixModule->index << " overlap!!!" << endl;
                return 0;
            }
        }
    }
    return 1;
}

void B_Tree::printTreePreorder(Node *node)
{ // for debug
    if (node == NULL)
    {
        return;
    }

    // cout << node->index << " ";
    if (node->parent != NULL)
        cout << " parent: " << node->parent->index << endl;
    else
        cout << "NO parent" << endl;
    printTreePreorder(node->left);
    printTreePreorder(node->right);
}

void B_Tree::printTree(const string &prefix, Node *parent, bool isLeft, bool isRoot)
{
    if (parent == nullptr)
        return;

    if (isRoot)
    {
        // cout << "─────";
    }
    else
    {
        // cout << prefix << (isLeft ? "L├────" : "R└───");
    }
    // cout << parent->index << endl;

    printTree(prefix + (isLeft ? " │   " : "    "),
              parent->left, true, false);
    printTree(prefix + (isLeft ? " │   " : "    "),
              parent->right, false, false);
}