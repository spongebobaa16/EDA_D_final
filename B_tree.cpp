#include "B_tree.h"
#include "Solver.h"
#include "util.h"
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

void B_Tree::SA(Solver &s)
{
    vector<Node *> best = Tree_vec;
    // vector<Node *> best;
    // best.assign(Tree_vec.begin(), Tree_vec.end());
    vector<Node *> temp_best = Tree_vec;
    s.floorplan(*this);
    float temp_cost = s.calculate_totalcost();
    float best_cost = temp_cost;
    s.Contour_H.clear();
    float new_cost = 0, delta_c;

    const int n = K * s.Modules.size(); // total number of uphill moves
    float T0 = initialTemp(s);
    float T = T0;
    float nmoves = 0, uphill = 0, reject = 0, reject_ratio;

    do
    {
        while (uphill < n && nmoves <= 2 * n)
        {
            new_cost = perturb(s);
            delta_c = new_cost - temp_cost;

            if (delta_c <= 0)
            { // down-hill move
                temp_best = Tree_vec;
                temp_cost = new_cost;
                cout << "DOWN HILLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL" << endl;
            }
            else
            { // uphill move
                if (accept(delta_c, T))
                { // decide if we should accept the new tree
                    cout << "UPHILL ACCEPTEDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << endl;
                    temp_best = Tree_vec;
                    temp_cost = new_cost;
                    uphill++;
                }
                else
                {
                    reject++;
                    cout << "REJECTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << endl;
                }
            }

            if (new_cost < best_cost)
            {
                best = Tree_vec;
                best_cost = new_cost;
            }
            cout << "best cost: " << best_cost << endl;
            nmoves++;
        }

        reject_ratio = reject / nmoves;
        nmoves = 0;
        uphill = 0;
        T = T < lambdatf * T0 ? 0.1 * T : ratio * T;
    } while (reject_ratio <= 0.95 && T >= epsilon);
    cout << "reject: " << reject << endl;
    cout << "nmoves: " << nmoves << endl;

    for (auto i : best)
        cout << i->index << endl;
    exit(0);
    for (auto i : best)
        cout << i->index << endl;

    Tree_vec = best;
}

bool B_Tree::accept(int delta_c, float T)
{
    float prob = exp(-delta_c / T);
    cout << "prob: " << prob << endl;
    double r = ((double)rand() / (RAND_MAX));
    cout << "r: " << r << endl;
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
        cout << "op1: rotate " << m1 << endl;
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

        cout << "op2: move " << m1 << " to " << m2 << endl;
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
        cout << "op3: swap " << m1 << " and " << m2 << endl;
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
            // cout<<"UPHILL"<<endl;
        }
        prev_cost = cost;
        // printTree();
    }

    float c_avg = uphill_cost / uphill_times; // calculates average difference of uphill moves
    float T0 = -c_avg / log(P);               // calculates initial temperature based on this formula, P being the initial probability of uphill moves
    cout << "initial temp: " << T0 << endl;
    return T0;
}

void B_Tree::prePlacedModule(Solver &s)
{
    printTree();
    vector<Module *> fixedModules;
    for (size_t i = s.Modules.size() - 1; i >= 0 && s.Modules[i]->fixed; --i)
        fixedModules.push_back(s.Modules[i]);
    for (auto i : fixedModules)
    {
        Node *fixedNode = Tree_vec[i->index], *_it = fixedNode->parent, *firstDominatedNode = 0;
        while (_it != 0)
        {
            if (s.Modules[_it->index]->isDominated(s.Modules[fixedNode->index]))
                break;
            _it = _it->parent;
        }
        firstDominatedNode = _it;
        cout << s.Modules[fixedNode->index]->location.x << ' ' << s.Modules[fixedNode->index]->location.y << endl;
        cout << s.Modules[firstDominatedNode->index]->location.x << ' ' << s.Modules[firstDominatedNode->index]->location.y << endl;
    }
}

void B_Tree::printTreePreorder(Node *node)
{ // for debug
    if (node == NULL)
    {
        return;
    }

    cout << node->index << " ";
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