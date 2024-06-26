#include "Coord.h"
#ifndef B_TREE_H
#define B_TREE_H
#include <vector>
#include <iostream>
#include "Module.h"
using namespace std;

// https://www.cprogramming.com/tutorial/lesson18.html
class Solver;

class Node
{
public:
    Node() {}
    Node(int _index) : parent(0), left(0), right(0), index(_index), _isRotated(0), WHtype(0) {}
    ~Node() {}
    bool isLeftChild()
    {
        if (parent == 0)
        {
            cout << "node " << index << " don't have parent but isLeftChild() is called!!!\n";
            return 0;
        }
        return (parent->left == this);
    }
    bool isChildOf(Node *_p) { return (this == _p->left || this == _p->right); }
    void rotate() { _isRotated = !_isRotated; }
    void changeWH() { WHtype++; }
    bool isRotated() { return _isRotated; }
    size_t depth() // depth are count from root(0)
    {
        size_t _depth = 0;
        Node *it = this;
        while (it->parent != 0)
        {
            ++_depth;
            it = it->parent;
        }
        return _depth;
    }
    bool getParent(vector<Module *> &_v, Node *&_ret, bool &_isretFixednplaced) // to obtain the first non-fixed parent // return true if as if this's "parent"'s left child
    {
        Node *_it = parent, *_prev = this; // _prev is to record which subtree does fixed node climb up from
        while (_it != 0)
        {
            if (!(_v[_it->index]->fixed && _v[_it->index]->fixed_status == 3)) // only skip those fixed and placed module
            {
                break;
            }
            if (_it->parent == 0) // i.e., _it == root
                break;
            _prev = _it;
            _it = _it->parent;
        }
        _ret = _it;
        _isretFixednplaced = (_v[_ret->index]->fixed_status == 3);
        return (_prev->isLeftChild());
    }
    Node *parent;
    Node *left;
    Node *right;
    int index;
    bool _isRotated;
    int WHtype; // keep adding one to choose different type of WL
};

class B_Tree
{
public:
    B_Tree() : root(0), dummy(new Node(-1)) {}
    ~B_Tree() {}

    void create_tree(const Solver &s); // initially create Node* for all blocks and push into Tree_vec.
    void insert(int index, int parent, bool parent_left, bool child_left);
    void remove(int index, bool child_left);
    // insert node(index) to be node(parent)'s parent_left child. (if parent_left: true -> left.   else -> right)
    // original child become new inserted node's child_left child. (if child_left: true -> left.   else -> right)

    // Node*   search(int index);    // == Tree_vec[index]

    // for SA
    void rotate(int index) { Tree_vec[index]->rotate(); };
    void changeWH(int index) { Tree_vec[index]->changeWH(); } // call this will render a new width and height(if existed)
    int Width(int i, const Solver &s);
    int Height(int i, const Solver &s);
    void swap(int index1, int index2);
    void move(int index1, int index2, bool parent_left, bool child_left); // move index1 to index2's left(right) child, depends on child_left == 1(0)
    void init_perturb(Solver &s);
    float perturb(Solver &s, float alpha, float beta);
    void initialTemp(Solver &s);
    float T0;
    void fastSA5(Solver &s, float beta);
    void SA(Solver &s, float beta);
    bool accept(int delta_c, float T);

    void copyTree(vector<Node *> &a, int size);  // copy nodes in Tree_vec to a
    void returnTree(vector<Node *> a, int size); // return nodes in a to Tree_vec
    bool prePlacedModule(Solver &s);
    void destroy_tree();
    void exchangableNode(Solver &s, Node *_node, Node *_fixed, vector<Node *> &D, size_t _specificDirection = 0); // search for those exchangableNode

    //  Debug
    void
    printTreePreorder(Solver &s, Node *node);
    void printTree(const string &prefix, Node *parent, bool isLeft, bool isRoot); // left subtree is on the top

    void printTree()
    {
        printTree("", root, false, true);
    }

    Node *root, *dummy;
    vector<Node *> Tree_vec;
};

#endif